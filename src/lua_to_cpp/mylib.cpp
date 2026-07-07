#include "lua_to_cpp/mylib.h"

static int l_add(lua_State* L)
{
    lua_Integer a = luaL_checkinteger(L, 1);
    lua_Integer b = luaL_checkinteger(L, 2);
    lua_pushinteger(L, a + b);
    return 1;
}

static int l_delete(lua_State* L)
{
    lua_Integer a = luaL_checkinteger(L, 1);
    lua_Integer b = luaL_checkinteger(L, 2);
    lua_pushinteger(L, a - b);
    return 1;
}

static int l_hello(lua_State* L)
{
    const char* name = luaL_checkstring(L, 1);
    lua_pushfstring(L, "Hello %s", name);
    return 1;
}

static const luaL_Reg mylib_funcs[] = {
    {"add", l_add},
    {"hello", l_hello},
    {"delete", l_delete},
    {nullptr, nullptr}
};

extern "C" int luaopen_mylib(lua_State* L)
{
    luaL_newlib(L, mylib_funcs);
    return 1;
}
