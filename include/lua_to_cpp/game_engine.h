#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <string>

struct PlayerConfig
{
    std::string name;
    int hp = 100;
    int attack = 10;
};

class GameEngine
{
public:
    explicit GameEngine(PlayerConfig config):config_(config){};//构造函数，初始化玩家配置
    static GameEngine create_player(const std::string& name);//静态方法，创建玩家对象
//类本身充当工厂，在不需要实例的情况下产出实例，这是创建型设计模式中最自然、最符合直觉的做法。
// auto engine = GameEngine::create_player(playerName);
// 这样创建的实例，没有生命周期管理问题，因为它是静态方法创建的，不需要手动释放。


    const std::string& name() const { return config_.name; }//获取玩家名称
    int hp() const { return config_.hp; }//获取玩家血量
    int attack() const { return config_.attack; }//获取玩家攻击力
    void damage(int amount);//伤害玩家
    void heal(int amount);//治疗玩家
    bool is_alive() const { return config_.hp > 0; }//判断玩家是否存活
    std::string to_string() const;//转换为字符串

private:
    PlayerConfig config_;//玩家配置 可被lua访问
};

#endif // GAME_ENGINE_H
