# LuaJIT + sol3 + lua-repl

Relocatable LuaJIT distribution with REPL (tab completion, pretty printing etc.) based on the excellent sol3 C++/Lua bindings.

* REPL provided by [lua-repl](https://github.com/hoelzro/lua-repl)
* [Sol3](https://github.com/ThePhD/sol2)

`hererocks` is required during build: `pipx install hererocks`

# Building and installing

Clone with `git clone --recursive https://github.com/Simon-L/luajit-sol3-repl`

Building and installing nng (not systemwide!) before everything is required.
```
cmake -S libs/nng -B libs/nng/build
cmake --build libs/nng/build
cmake --install libs/nng/build --prefix prefix
```

Building the hererocks target first is required.
```
cmake -Bbuild
cmake --build build -j16 -t hererocks
cmake --build build -j16
```

See the [BUILDING](BUILDING.md) document.

# Usage

This should be a pretty standard *decent* lua interpreter:
```
./soljit
```

There is a single route currently: `http://127.0.0.1:8888/api/eval` send POST request to this endpoint to execute the code in the REPL.  
Request must contain the follow JSON data:
```json
{
  "code": "<base64 encoded string>"
}
```

# Contributing

See the [CONTRIBUTING](CONTRIBUTING.md) document.

# Licensing

MIT
