#include "lua_to_cpp/game_engine.h"

GameEngine GameEngine::create_player(const std::string& name)
{
    return GameEngine({name, 100, 10});
}

void GameEngine::damage(int amount)
{
    config_.hp -= amount;
    if (config_.hp < 0)
        config_.hp = 0;
}

void GameEngine::heal(int amount)
{
    config_.hp += amount;
}

std::string GameEngine::to_string() const
{
    return config_.name + " hp=" + std::to_string(config_.hp) +
           " attack=" + std::to_string(config_.attack);
}
