#include "lua_to_cpp/register_bindings.h"

#include "lua_to_cpp/game_engine.h"

#include <sol/sol.hpp>
#include <string>

namespace {

//将table对象中的属性转换为PlayerConfig对象
PlayerConfig config_from_table(const sol::table& config) {
    PlayerConfig cfg;
    //"unknown" 字面量推导出的是 const char*，与 PlayerConfig::name 的 std::string 不一致，
    // 因此必须显式指定 <std::string>。"unknown" 字面量推导出的是 const char*，
    // 与 PlayerConfig::name 的 std::string 不一致，因此必须显式指定 <std::string>。
    cfg.name = config.get_or<std::string>("name" , "unknown");
    cfg.hp = config.get_or("hp" , static_cast<int>(100));
    cfg.attack = config.get_or("attack" , static_cast<int>(10));
    return cfg;
}

//将GameEngine对象中的属性转换为table对象，方便Lua访问
sol::table stats_to_table(sol::state_view lua, const GameEngine& player)
{
    sol::table stats = lua.create_table();//需要lua造table对象，不能直接在C++中造table对象，因为Lua的table对象是Lua的，不是C++的。
    stats["name"] = player.name();
    stats["hp"] = player.hp();
    stats["attack"] = player.attack();
    stats["alive"] = player.is_alive();
    return stats;
}

void register_cpp_bindings(sol::state_view lua)
{
    //注册GameEngine类，让Lua能够持有对象，访问GameEngine类中的方法和属性
    lua.new_usertype<GameEngine>(
        "GameEngine",
        sol::no_constructor,//不构造GameEngine对象，要求必须要用C++的构造函数来进行构造，避免Lua直接构造GameEngine对象导致出现问题
        "name", sol::readonly(&GameEngine::name),//只读name属性，避免Lua直接修改name属性导致出现问题
        "hp", sol::readonly(&GameEngine::hp),
        "attack", sol::readonly(&GameEngine::attack),
        "damage", &GameEngine::damage,//&GameEngine::damage 是函数指针，指向GameEngine::damage函数，可以被Lua调用
        "heal", &GameEngine::heal,
        "is_alive", &GameEngine::is_alive,
        "to_string", &GameEngine::to_string);
    
    sol::table module = lua.create_table();//创建一个table对象
    module["add"] = [](int a, int b) { return a + b; };
    module["hello"] = [](const std::string& name) { return "Hello " + name; };
    module["delete"] = sol::as_function([](int a, int b) { return a - b; });
    //将sol::as_function转换为函数对象，避免编译/宏冲突风险，提高代码安全性
    module["subtract"] = sol::as_function([](int a, int b) { return a - b; });
    module["create_player"] = &GameEngine::create_player;
    //必须包一层 GameEngine(...)，因为 Lua 要的是玩家对象，不是配置结构体。
    module["create_from_config"] = [](const sol::table& config) {
        return GameEngine(config_from_table(config));
    };
    module["get_player_stats"] = [lua](const GameEngine& player) {//捕获lua变量，避免lambda捕获不到lua变量导致出现问题
        return stats_to_table(lua, player);
    };

    lua["package"]["loaded"]["mylib"] = module;
    //将module对象注册到Lua中，让Lua能够访问module对象中的方法和属性，
    // 并将其赋值给lua["package"]["loaded"]["mylib"]
}

} // namespace

extern "C" int luaopen_mylib(lua_State* L)
{
    sol::state_view lua(L);//不拥有
    //创建一个sol::state_view对象，用于访问Lua状态，相当于一个Lua的上下文环境，提供了Lua的API，可以访问Lua的变量、函数、表等。
    register_cpp_bindings(lua);//注册cpp绑定
    return sol::stack::push(L, lua["package"]["loaded"]["mylib"]);//将lua["package"]["loaded"]["mylib"]压入栈中
}
