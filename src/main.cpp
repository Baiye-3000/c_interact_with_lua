#include <iostream>
#include <lua.hpp>
#include "mylib.h"

int main()
{
    lua_State* L = luaL_newstate();//
    luaL_openlibs(L);// 打开 Lua 标准库


    // 将 C++ 库注册为 Lua 模块，脚本中可通过 require("mylib") 调用
    luaL_requiref(L,"mylib",luaopen_mylib,1);
    lua_pop(L,1);// 弹出栈顶的模块 必须调用，否则会栈溢出     

    if (luaL_dofile(L, "lua/script.lua") !=  LUA_OK)
    {
        std::cerr << "Failed to load lua script: " << lua_tostring(L,-1) << std::endl;
        lua_close(L);
        return -1;    
    }
    std::cout << "Lua script loaded successfully!" << std::endl;

    // 执行 Lua 脚本
    lua_getglobal(L,"lua_add");
    lua_pushnumber(L,3);
    lua_pushnumber(L,5);
    if(lua_pcall(L,2,1,0) != LUA_OK)
    {
        std::cerr << lua_tostring(L,-1) <<std::endl;
        lua_close(L);
        return -1;
    }
    std::cout << "C++ calls Lua add(3,5) = " << lua_tonumber(L,-1) << std::endl;
    lua_pop(L,1);

    lua_getglobal(L,"lua_hello");
    lua_pushstring(L,"World");
    if(lua_pcall(L,1,0,0) != LUA_OK)
    {
        std::cerr << lua_tostring(L, -1) << std::endl;
        lua_close(L);
        return -1;
    }
    lua_getglobal(L,"lua_delete");
    lua_pushnumber(L,3);
    lua_pushnumber(L,5);
    if(lua_pcall(L,2,1,0) != LUA_OK)
    {
        std::cerr << lua_tostring(L,-1) <<std::endl;
        lua_close(L);
        return -1;
    }
    std::cout << "C++ calls Lua delete(3,5) = " << lua_tonumber(L,-1) << std::endl;
    lua_pop(L,1);



    lua_close(L);
    return 0;
}
