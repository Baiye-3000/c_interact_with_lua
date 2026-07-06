#ifndef MYLIB_H
#define MYLIB_H

#include <lua.hpp>

extern "C" {
    int luaopen_mylib(lua_State* L);// 导出函数
}
//extern "C" 保证 Lua 能找到 luaopen_mylib，而 luaopen_mylib 是 require("mylib") 时自动调用的模块初始化函数。
#endif // MYLIB_H
