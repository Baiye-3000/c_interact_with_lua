-- 供 C++ 调用的 Lua 回调（模块 + 全局兼容）
local M = {}

function M.add(a, b)
    return a + b
end

function M.greet(name)
    print("Hello " .. name .. " (from Lua)")
end

function M.subtract(a, b)
    return a - b
end

-- 阶段 4b：Lua 回调接收 C++ 对象（usertype）
function M.on_player_hit(player, damage)
    print("Lua received C++ player:", player:to_string(), "damage =", damage)
    player:damage(damage)
    return player:is_alive()
end

-- 兼容阶段 1-3 的全局函数名
function lua_add(a, b)
    return M.add(a, b)
end

function lua_hello(name)
    M.greet(name)
end

function lua_delete(a, b)
    return M.subtract(a, b)
end

return M
