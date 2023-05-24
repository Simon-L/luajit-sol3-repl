#include "lib.hpp"
#include <cstdio>

#include <sol/sol.hpp>
#include <cassert>

#include <cxxopts.hpp>

#include "tcpserver.hpp"
#include <iostream>

using namespace std;

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

    // Initialize server socket..
    TCPServer<> tcpServer;

    // When a new client connected:
    tcpServer.onNewConnection = [&](TCPSocket<> *newClient) {
        cout << "New client: [";
        cout << newClient->remoteAddress() << ":" << newClient->remotePort() << "]" << endl;

        // newClient->onMessageReceived = [newClient](string message) {
        //     cout << newClient->remoteAddress() << ":" << newClient->remotePort() << " => " << message << endl;
        //     newClient->Send("OK!");
        // };

        // If you want to use raw bytes
        newClient->onRawMessageReceived = [newClient](const char* message, int length) {
            cout << newClient->remoteAddress() << ":" << newClient->remotePort() << " => " << message << "(" << length << ")" << endl;
            newClient->Send("OK!");
        };

        newClient->onSocketClosed = [newClient](int errorCode) {
            cout << "Socket closed:" << newClient->remoteAddress() << ":" << newClient->remotePort() << " -> " << errorCode << endl;
            cout << flush;
        };
    };

    // Bind the server to a port.
    tcpServer.Bind(8888, [](int errorCode, string errorMessage) {
        // BINDING FAILED:
        cout << errorCode << " : " << errorMessage << endl;
    });

    // Start Listening the server.
    tcpServer.Listen([](int errorCode, string errorMessage) {
        // LISTENING FAILED:
        cout << errorCode << " : " << errorMessage << endl;
    });

    if ((result.count("interactive")) || !(result.count("script"))) lua.script_file("../lua/rep.lua");

    tcpServer.Close();

    return 0;
}
