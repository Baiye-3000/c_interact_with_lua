-- Lua 调用 C++ 库中的函数
local mylib = require("mylib")

print("Lua calls C++ add(10, 20) =", mylib.add(10, 20))
print("Lua calls C++ hello('Lua') =", mylib.hello("Lua"))
print("Lua calls C++ delete(10, 20) =", mylib.delete(10, 20))

-- 阶段 4a：Lua table → C++ 对象
print("--- Phase 4a: Lua table -> C++ object ---")
local player = mylib.create_from_config({ name = "Archer", hp = 80, attack = 15 })
print("Created player:", player:to_string())

player:damage(20)
print("After damage(20):", player:to_string())

local stats = mylib.get_player_stats(player)
print("Stats table from C++:", stats.name, stats.hp, stats.attack, stats.alive)
