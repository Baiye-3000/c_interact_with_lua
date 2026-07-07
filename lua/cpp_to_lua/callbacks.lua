-- 供 C++ 调用的 Lua 回调函数
function lua_add(a, b)
    return a + b
end

function lua_hello(name)
    print("Hello " .. name .. " (from Lua)")
end

function lua_delete(a, b)
    return a - b
end
