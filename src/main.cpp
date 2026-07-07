#include <iostream>
#include <lua.hpp>

#include "cpp_to_lua/script_bridge.h"
#include "lua_to_cpp/mylib.h"

int main()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    luaL_requiref(L, "mylib", luaopen_mylib, 1);
    lua_pop(L, 1);
// 将 C++ 库注册为 Lua 模块，脚本中可通过 require("mylib") 调用
// 弹出栈顶的模块 必须调用，否则会栈溢出     
   
    //lua调用c++函数
    if (luaL_dofile(L, "lua/lua_to_cpp/demo_call_cpp.lua") != LUA_OK)
    {
        std::cerr << "Failed to load lua script: " << lua_tostring(L, -1) << std::endl;
        lua_close(L);
        return -1;
    }

    //加载Lua脚本，供C++调用
    if (!load_lua_callbacks(L, "lua/cpp_to_lua/callbacks.lua"))
    {
        lua_close(L);
        return -1;
    }
    std::cout << "Lua script loaded successfully!" << std::endl;

    if (!run_cpp_to_lua_call(L))
    {
        lua_close(L);
        return -1;
    }

    lua_close(L);
    return 0;
}
