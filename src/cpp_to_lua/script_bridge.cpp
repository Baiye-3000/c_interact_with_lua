#include "cpp_to_lua/script_bridge.h"

#include <iostream>

// 调用Lua的数字结果函数
static bool call_lua_number_result(sol::state_view lua, const char* func,
                                   double arg1, double arg2, double& result)
{
    sol::protected_function fn = lua[func];
    sol::protected_function_result ret = fn(arg1, arg2);//可以把它理解成：一次 Lua 调用的“结果 + 错误状态 + 栈上返回值”的临时容器。
    if (!ret.valid())
    {
        sol::error err = ret;
        std::cerr << "Failed to call Lua function: " << err.what() << std::endl;
        return false;
    }
    result = ret.get<double>();
    return true;
}
// 调用Lua的void函数
static bool call_lua_void(sol::state_view lua, const char* func, const char* arg)
{
    sol::protected_function fn = lua[func];
    sol::protected_function_result ret = fn(arg);
    if (!ret.valid())
    {
        sol::error err = ret;
        std::cerr << err.what() << std::endl;
        return false;
    }
    return true;
}
// 加载Lua回调模块
bool load_lua_callbacks(sol::state_view lua, const char* path)
{
    sol::protected_function_result ret = lua.script_file(path);
    if (!ret.valid())
    {
        sol::error err = ret;
        std::cerr << "Failed to load lua script: " << err.what() << std::endl;
        return false;
    }

    sol::table callbacks = ret;//将返回值转换为table对象，进行了隐式转换，将sol::protected_function_result转换为sol::table
    lua["cpp_callbacks"] = callbacks;//将table对象赋值给lua["cpp_callbacks"]    //将table对象赋值给lua["cpp_callbacks"]，方便Lua访问
    return true;//返回true，表示加载成功
}
// 执行C++ -> Lua的回调
bool run_cpp_to_lua_call(sol::state_view lua)
{
    double result = 0.0;

    if (!call_lua_number_result(lua, "lua_add", 3, 5, result))
        {return false;} 
    std::cout << "C++ calls Lua add(3,5) = " << result << std::endl;

    if (!call_lua_void(lua, "lua_hello", "World"))
        {return false;} 

    if (!call_lua_number_result(lua, "lua_delete", 3, 5, result))
        {return false;} 
    std::cout << "C++ calls Lua delete(3,5) = " << result << std::endl;

    return true; // 返回 true，表示调用成功
}

// 执行C++ -> Lua的回调，传递C++对象
bool run_phase4_object_callback(sol::state_view lua, GameEngine& player)
{
    std::cout << "--- Phase 4b: C++ object -> Lua callback ---" << std::endl;
    std::cout << "C++ before Lua hit: " << player.to_string() << std::endl;

    sol::table callbacks = lua["cpp_callbacks"];//获取Lua回调模块
    sol::protected_function on_hit = callbacks["on_player_hit"];//获取Lua回调函数
    sol::protected_function_result ret = on_hit(player, 30);//调用Lua回调函数
    if (!ret.valid())
    {
        sol::error err = ret;
        std::cerr << "Failed to call on_player_hit: " << err.what() << std::endl;
        return false;//返回false，表示调用失败
    }

    bool alive = ret.get<bool>();//获取返回值
    std::cout << "C++ after Lua hit: " << player.to_string()
              << ", alive = " << std::boolalpha << alive << std::endl;
    return true;//返回true，表示调用成功
}
