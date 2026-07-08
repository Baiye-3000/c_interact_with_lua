#include <iostream>
#include <sol/sol.hpp>

#include "cpp_to_lua/script_bridge.h"
#include "lua_to_cpp/game_engine.h"
#include "lua_to_cpp/register_bindings.h"

int main()
{
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package);//打开base和package库

    std::cout << "[Step 1] Init sol::state + register mylib" << std::endl;//注册mylib库
    luaL_requiref(lua.lua_state(), "mylib", luaopen_mylib, 1);//luaL_requiref函数用于在Lua中加载和注册C/C++库
    lua_pop(lua.lua_state(), 1);

    std::cout << "[Step 2] Lua -> C++ basic + Phase 4a (table/usertype)" << std::endl;
    sol::protected_function_result demo =
        lua.script_file("lua/lua_to_cpp/demo_call_cpp.lua");
    if (!demo.valid())
    {
        sol::error err = demo;
        std::cerr << "Failed to load lua script: " << err.what() << std::endl;
        return -1;
    }


    // 执行Lua回调模块，提供给C++调用
    std::cout << "[Step 3] Load Lua callbacks module" << std::endl;
    if (!load_lua_callbacks(lua, "lua/cpp_to_lua/callbacks.lua"))
       {return -1;} 
    std::cout << "Lua script loaded successfully!" << std::endl;

    std::cout << "[Step 4] C++ -> Lua basic callbacks" << std::endl;
    if (!run_cpp_to_lua_call(lua))
        {return -1;} 

    std::cout << "[Step 5] C++ -> Lua with C++ object (Phase 4b)" << std::endl;
    GameEngine warrior = GameEngine::create_player("Warrior");
    warrior.damage(10);
    if (!run_phase4_object_callback(lua, warrior))
        {return -1;}    

    return 0;
}
