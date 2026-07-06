# Lua + C++ 互调示例

本项目演示 **C++ 与 Lua 双向调用** 的最小完整流程：

- **Lua 调用 C++**：通过 `require("mylib")` 使用 C++ 库中的函数
- **C++ 调用 Lua**：通过 Lua C API 调用脚本中定义的全局函数

---

## 环境要求

| 依赖 | 说明 |
|------|------|
| C++ 编译器 | 支持 C++11 及以上（g++ / clang++） |
| CMake | 3.10 及以上 |
| Lua | 5.x 开发库（头文件 + 库文件） |

---

## 快速开始

```bash
cd lua_cpp_demo
./run.sh
```

`run.sh` 会自动完成：**CMake 配置 → 编译 → 运行**，并在项目根目录启动程序（确保能找到 `lua/script.lua`）。

---

## 手动构建与运行

```bash
cd lua_cpp_demo
mkdir -p build && cd build
cmake ..
make
cd .. && ./build/main
```

> 必须在 `lua_cpp_demo` 目录下运行 `./build/main`，因为程序通过相对路径 `lua/script.lua` 加载脚本。

---

## 预期输出

```
Lua calls C++ add(10, 20) = 30
Lua calls C++ hello('Lua') = Hello Lua
Lua calls C++ delete(10, 20) = -10
Lua script loaded successfully!
C++ calls Lua add(3,5) = 8
Hello World (from Lua)
C++ calls Lua delete(3,5) = -2
```

---

## 项目结构

```
lua_cpp_demo/
├── include/
│   └── mylib.h          # C++ 库头文件，声明模块入口 luaopen_mylib
├── src/
│   ├── main.cpp         # 主程序：初始化 Lua、注册库、执行脚本、调用 Lua 函数
│   └── mylib.cpp        # C++ 库实现：add / delete / hello
├── lua/
│   └── script.lua       # Lua 脚本：调用 C++ 库，并定义供 C++ 调用的函数
├── CMakeLists.txt       # 构建配置
├── run.sh               # 一键构建运行脚本
└── README.md            # 本文档
```

---

## 功能说明

### 1. C++ 库（mylib）

C++ 函数封装为 Lua 模块 `mylib`，Lua 侧通过 `require("mylib")` 加载。

| Lua 调用 | C++ 实现 | 功能 |
|----------|----------|------|
| `mylib.add(a, b)` | `l_add` | 两整数相加，返回 `a + b` |
| `mylib.delete(a, b)` | `l_delete` | 两整数相减，返回 `a - b` |
| `mylib.hello(name)` | `l_hello` | 返回格式化字符串 `"Hello {name}"` |

模块注册流程：

1. `main.cpp` 调用 `luaL_requiref(L, "mylib", luaopen_mylib, 1)` 将库注册到 `package.loaded`
2. `luaopen_mylib` 通过 `luaL_newlib` 创建模块表 `{ add, delete, hello }`
3. Lua 脚本执行 `require("mylib")` 时直接获取已注册的模块

### 2. Lua 脚本（script.lua）

脚本中定义了供 C++ 调用的全局函数：

| C++ 调用 | Lua 函数 | 功能 |
|----------|----------|------|
| `lua_add(3, 5)` | `lua_add(a, b)` | 返回 `a + b` |
| `lua_hello("World")` | `lua_hello(name)` | 打印问候语 |
| `lua_delete(3, 5)` | `lua_delete(a, b)` | 返回 `a - b` |

### 3. 主程序（main.cpp）

程序按以下顺序执行：

```
创建 Lua 虚拟机
    ↓
加载 Lua 标准库（含 require、print 等）
    ↓
注册 C++ 库 mylib（luaL_requiref）
    ↓
执行 lua/script.lua（Lua 调用 C++）
    ↓
C++ 调用 Lua 全局函数 lua_add / lua_hello / lua_delete
    ↓
关闭 Lua 虚拟机，程序退出
```

---

## 运行流程详解

### Lua 调用 C++

```lua
local mylib = require("mylib")
mylib.add(10, 20)   -- 调用 C++ 的 l_add
```

```
require("mylib")
  → 从 package.loaded 获取 C++ 注册的模块表
  → mylib.add(10, 20)
  → Lua 将参数压栈，跳转到 C++ 函数 l_add
  → C++ 计算结果并压栈返回
  → Lua 拿到返回值
```

### C++ 调用 Lua

```cpp
lua_getglobal(L, "lua_add");   // 获取 Lua 全局函数
lua_pushnumber(L, 3);          // 压入第 1 个参数
lua_pushnumber(L, 5);          // 压入第 2 个参数
lua_pcall(L, 2, 1, 0);         // 调用：2 个参数，期望 1 个返回值
lua_tonumber(L, -1);           // 读取栈顶返回值
```

```
lua_getglobal → 将 Lua 函数压栈
lua_push*     → 将参数依次压栈
lua_pcall     → 执行 Lua 函数，结果压回栈顶
lua_to*       → C++ 读取返回值
lua_pop       → 清理栈
```

---

## 如何扩展

### 新增一个 C++ 函数供 Lua 调用

以添加 `multiply(a, b)` 为例：

**1. 在 `src/mylib.cpp` 中实现绑定函数：**

```cpp
static int l_multiply(lua_State* L)
{
    lua_Integer a = luaL_checkinteger(L, 1);
    lua_Integer b = luaL_checkinteger(L, 2);
    lua_pushinteger(L, a * b);
    return 1;
}
```

**2. 注册到函数表：**

```cpp
static const luaL_Reg mylib_funcs[] = {
    {"add", l_add},
    {"delete", l_delete},
    {"hello", l_hello},
    {"multiply", l_multiply},   // 新增
    {nullptr, nullptr}
};
```

**3. 在 `lua/script.lua` 中调用：**

```lua
print(mylib.multiply(3, 4))   -- 输出 12
```

重新编译运行即可，无需修改 `main.cpp`。

### 新增一个 Lua 函数供 C++ 调用

**1. 在 `lua/script.lua` 中定义全局函数：**

```lua
function lua_multiply(a, b)
    return a * b
end
```

**2. 在 `src/main.cpp` 中调用：**

```cpp
lua_getglobal(L, "lua_multiply");
lua_pushnumber(L, 3);
lua_pushnumber(L, 4);
if (lua_pcall(L, 2, 1, 0) != LUA_OK) { /* 错误处理 */ }
std::cout << lua_tonumber(L, -1) << std::endl;
lua_pop(L, 1);
```

---

## 关键 API 说明

| API | 用途 |
|-----|------|
| `luaL_newstate()` | 创建 Lua 虚拟机 |
| `luaL_openlibs(L)` | 加载 Lua 标准库 |
| `luaL_requiref(L, name, openf, glb)` | 注册 C/C++ 模块到 `package.loaded` |
| `lua_pop(L, n)` | 弹出栈顶 n 个元素，保持栈平衡 |
| `luaL_dofile(L, path)` | 加载并执行 Lua 脚本文件 |
| `lua_getglobal(L, name)` | 获取 Lua 全局变量/函数并压栈 |
| `lua_pcall(L, nargs, nresults, errfunc)` | 保护模式调用栈上的 Lua 函数 |
| `luaL_checkinteger(L, idx)` | 从栈上读取整数参数（类型不对则报错） |
| `luaL_checkstring(L, idx)` | 从栈上读取字符串参数 |
| `lua_pushinteger(L, n)` | 将整数压入栈作为返回值 |
| `lua_pushfstring(L, fmt, ...)` | 将格式化字符串压入栈作为返回值 |
| `luaopen_mylib(L)` | C++ 模块入口，命名规则为 `luaopen_` + 模块名 |

---

## 常见问题

### 找不到 `lua/script.lua`

```
Failed to load lua script: cannot open lua/script.lua
```

**原因**：程序使用相对路径加载脚本，必须在 `lua_cpp_demo` 目录下运行。

**解决**：使用 `./run.sh`，或手动 `cd lua_cpp_demo && ./build/main`。

### 编译报错 `找不到 lua.hpp`

**原因**：未安装 Lua 开发包。

**解决**：安装 Lua 开发库，或确保 CMake 的 `find_package(Lua)` 能找到头文件和库文件。

### `require("mylib")` 报错

**原因**：C++ 侧未注册模块，或 `luaopen_mylib` 命名/链接有问题。

**解决**：确认 `main.cpp` 中在 `luaL_dofile` 之前调用了 `luaL_requiref(L, "mylib", luaopen_mylib, 1)`，且 `mylib.h` 中使用了 `extern "C"`。

### 调用 Lua 函数失败

**原因**：在 `luaL_dofile` 之前调用，或函数名拼写不一致。

**解决**：确保先执行脚本（函数已定义），再 `lua_getglobal`；C++ 中的名字与 Lua 中 `function` 名完全一致。

---

## 构建说明

`CMakeLists.txt` 将项目编译为：

- `libmylib.a` — C++ 静态库（Lua 绑定函数）
- `main` — 可执行文件（链接 mylib + Lua 库）

采用静态库方式链接，通过 `luaL_requiref` 在运行时注册到 Lua，无需额外的 `.so` 动态库文件。
