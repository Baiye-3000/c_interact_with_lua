#ifndef REGISTER_BINDINGS_H
#define REGISTER_BINDINGS_H

#include <lua.hpp>

extern "C" {
int luaopen_mylib(lua_State* L);
}

#endif // REGISTER_BINDINGS_H
