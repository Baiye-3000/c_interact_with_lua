#include "cpp_to_lua/script_bridge.h"
#include <iostream>

static bool call_lua_number_result(lua_State* L, const char* func,
                                   double arg1, double arg2, double& result)
{
    lua_getglobal(L, func);
    lua_pushnumber(L, arg1);
    lua_pushnumber(L, arg2);
    if (lua_pcall(L, 2, 1, 0) != LUA_OK)
    {
        std::cerr << lua_tostring(L, -1) << std::endl;
        return false;
    }
    result = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return true;
}

static bool call_lua_void(lua_State* L, const char* func, const char* arg)
{
    lua_getglobal(L, func);
    lua_pushstring(L, arg);
    if (lua_pcall(L, 1, 0, 0) != LUA_OK)
    {
        std::cerr << lua_tostring(L, -1) << std::endl;
        return false;
    }
    return true;
}

bool load_lua_callbacks(lua_State* L, const char* path)
{
    if (luaL_dofile(L, path) != LUA_OK)
    {
        std::cerr << "Failed to load lua script: " << lua_tostring(L, -1) << std::endl;
        return false;
    }
    return true;
}

bool run_cpp_to_lua_call(lua_State* L)
{
    double result = 0.0;

    if (!call_lua_number_result(L, "lua_add", 3, 5, result))
    {
        return false;
    }
    std::cout << "C++ calls Lua add(3,5) = " << result << std::endl;

    if (!call_lua_void(L, "lua_hello", "World"))
    {
        return false;
    }

    if (!call_lua_number_result(L, "lua_delete", 3, 5, result))
    {
        return false;
    }
    std::cout << "C++ calls Lua delete(3,5) = " << result << std::endl;

    return true;
}
