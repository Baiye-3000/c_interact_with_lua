-- Lua 调用 C++ 库中的函数
local mylib = require("mylib")

print("Lua calls C++ add(10, 20) =", mylib.add(10, 20))
print("Lua calls C++ hello('Lua') =", mylib.hello("Lua"))
print("Lua calls C++ delete(10, 20) =", mylib.delete(10, 20))

-- 供 C++ 调用的 Lua 函数
function lua_add(a, b)
    return a + b
end

function lua_hello(name)
    print("Hello " .. name .. " (from Lua)")
end

function lua_delete(a, b)
    return a - b
end
