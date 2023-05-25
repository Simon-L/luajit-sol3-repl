#include "lib.hpp"
#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <sol/sol.hpp>
#include <cxxopts.hpp>
#include <nngpp/nngpp.h>
#include <nngpp/http/http.h>

#define REST_URL "http://127.0.0.1:8888/api/eval"

void rest_handle(nng_aio* a) try {
	nng::aio_view aio = a;
	nng::http::req_view req = aio.get_input<nng_http_req>(0);

	auto data = req.get_data();

	printf("Got data: %s with size %u\n", data.data(), data.size());

	auto res = nng::http::make_res();
	res.set_status(nng::http::status::ok);
	res.set_header("Content-Length", "0");
	char buf[128];
	snprintf(buf, sizeof(buf), "Oh, hello");
	res.set_reason( buf );
	aio.set_output(0,res.release());
	aio.finish();
	aio = nullptr;
	printf("Hey?\n");
}
catch( const nng::exception& e ) {
	fprintf(stderr, "rest_handle: %s: %s\n", e.who(), e.what());
	exit(1);
}
catch( ... ) {
	fprintf(stderr, "rest_handle: unknown exception\n");
	exit(1);
}

static void print_version(void)
{
  fputs(LUAJIT_VERSION " -- " LUAJIT_COPYRIGHT ". " LUAJIT_URL "\n", stdout);
}

static void print_jit_status(lua_State *L)
{
  int n;
  const char *s;
  lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
  lua_getfield(L, -1, "jit");  /* Get jit.* module table. */
  lua_remove(L, -2);
  lua_getfield(L, -1, "status");
  lua_remove(L, -2);
  n = lua_gettop(L);
  lua_call(L, 0, LUA_MULTRET);
  fputs(lua_toboolean(L, n) ? "JIT: ON" : "JIT: OFF", stdout);
  for (n++; (s = lua_tostring(L, n)); n++) {
    putc(' ', stdout);
    fputs(s, stdout);
  }
  putc('\n', stdout);
  lua_settop(L, 0);  /* clear stack */
}

int main(int argc, const char* argv[]) {
    cxxopts::Options options("soljit", "LuaJIT interpreter");

    options
      .set_tab_expansion()
      .allow_unrecognised_options();
    options.add_options()
      ("i,interactive", "Enter interactive mode after executing 'script'.", cxxopts::value<bool>()->default_value("false"))
      ("e,execute", "Execute string 'chunk'.", cxxopts::value<std::string>())
      ("l,library", "Require library 'name'.", cxxopts::value<std::vector<std::string>>())
      ("h,help", "Print usage")
      ("script", "script filename", cxxopts::value<std::string>())
      ("script_args", "script arguments", cxxopts::value<std::vector<std::string>>());

    options.parse_positional({"script", "script_args"});
    options.positional_help("script [arg]");

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
      std::cout << options.help() << std::endl;
      exit(0);
    }

    sol::state lua;
    lua.open_libraries();
    print_version();
    print_jit_status(lua);

    if (result.count("script_args")) {
      std::vector<std::string> script_args = result["script_args"].as<std::vector<std::string>>();
      lua.set("arg", sol::as_table(script_args));
    }

    if (result.count("script"))
    {
      std::string script = result["script"].as<std::string>();
      lua["arg"][0] = script;
      lua.script_file(script);
    } else
    {
      lua.set("arg", sol::nil);
    }

    nng::url url(REST_URL);
    nng::http::server server(url);
    nng::http::handler handler( url->u_path, rest_handle );
    handler.set_method( nng::http::verb::post );
    handler.collect_body(true, 1024 * 128);
    server.add_handler( std::move(handler) );
    try {
    	server.start();
      printf("rest started!\n");
    }
    catch( const nng::exception& e ) {
    	// who() is the name of the nng function that produced the error
    	// what() is a description of the error code
    	printf( "%s: %s\n", e.who(), e.what() );
    	return 1;
    }

    if ((result.count("interactive")) || !(result.count("script"))) lua.script_file("../lua/rep.lua");

    return 0;
}
