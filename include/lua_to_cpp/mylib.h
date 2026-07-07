#ifndef MYLIB_H
#define MYLIB_H

#include <lua.hpp>

extern "C" {
int luaopen_mylib(lua_State* L);
}

#endif // MYLIB_H
