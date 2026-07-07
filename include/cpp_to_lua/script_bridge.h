#ifndef SCRIPT_BRIDGE_H
#define SCRIPT_BRIDGE_H

#include <lua.hpp>

//加载Lua脚本，供C++调用
bool load_lua_callbacks(lua_State* L, const char* path);
//执行Lua脚本，供C++调用
bool run_cpp_to_lua_call(lua_State* L);


#endif // SCRIPT_BRIDGE_H
