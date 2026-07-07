-- Lua 调用 C++ 库中的函数
local mylib = require("mylib")

print("Lua calls C++ add(10, 20) =", mylib.add(10, 20))
print("Lua calls C++ hello('Lua') =", mylib.hello("Lua"))
print("Lua calls C++ delete(10, 20) =", mylib.delete(10, 20))
