# Lua + C++ 互调示例

本项目演示 **C++ 与 Lua 双向调用** 的最小完整流程，代码按调用方向拆分目录：

- **Lua → C++**：`lua/lua_to_cpp/` + `src/lua_to_cpp/`
- **C++ → Lua**：`lua/cpp_to_lua/` + `src/cpp_to_lua/`

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

`run.sh` 会自动完成：**CMake 配置 → 编译 → 运行**，并在项目根目录启动程序。

---

## 手动构建与运行

```bash
cd lua_cpp_demo
mkdir -p build && cd build
cmake ..
make
cd .. && ./build/main
```

> 必须在 `lua_cpp_demo` 目录下运行 `./build/main`，程序通过相对路径加载 `lua/` 下的脚本。

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
│   ├── lua_to_cpp/
│   │   └── mylib.h              # C++ 库头文件，声明 luaopen_mylib
│   └── cpp_to_lua/
│       └── script_bridge.h      # C++ 调用 Lua 的桥接接口
├── src/
│   ├── main.cpp                 # 主程序：仅负责流程编排
│   ├── lua_to_cpp/
│   │   └── mylib.cpp            # C++ 库实现：add / delete / hello
│   └── cpp_to_lua/
│       └── script_bridge.cpp    # 加载回调脚本、调用 Lua 函数
├── lua/
│   ├── lua_to_cpp/
│   │   └── demo_call_cpp.lua    # Lua 调用 C++ 示例脚本
│   └── cpp_to_lua/
│       └── callbacks.lua        # 供 C++ 调用的 Lua 回调函数
├── CMakeLists.txt
├── run.sh
└── README.md
```

---

## 功能说明

### 1. Lua → C++（`lua_to_cpp`）

| 文件 | 职责 |
|------|------|
| `src/lua_to_cpp/mylib.cpp` | 将 C++ 函数绑定为 Lua 模块 `mylib` |
| `lua/lua_to_cpp/demo_call_cpp.lua` | 通过 `require("mylib")` 调用 C++ 函数 |

| Lua 调用 | C++ 实现 | 功能 |
|----------|----------|------|
| `mylib.add(a, b)` | `l_add` | 返回 `a + b` |
| `mylib.delete(a, b)` | `l_delete` | 返回 `a - b` |
| `mylib.hello(name)` | `l_hello` | 返回 `"Hello {name}"` |

### 2. C++ → Lua（`cpp_to_lua`）

| 文件 | 职责 |
|------|------|
| `lua/cpp_to_lua/callbacks.lua` | 定义全局回调函数 |
| `src/cpp_to_lua/script_bridge.cpp` | 加载脚本并调用 Lua 函数 |

| C++ 调用 | Lua 函数 | 功能 |
|----------|----------|------|
| `run_cpp_to_lua_demo()` 内 | `lua_add(3, 5)` | 返回 `8` |
| 同上 | `lua_hello("World")` | 打印问候语 |
| 同上 | `lua_delete(3, 5)` | 返回 `-2` |

### 3. 主程序编排（`main.cpp`）

```
创建 Lua 虚拟机
    ↓
加载标准库 + 注册 mylib
    ↓
执行 lua/lua_to_cpp/demo_call_cpp.lua   （Lua 调 C++）
    ↓
load_lua_callbacks("lua/cpp_to_lua/callbacks.lua")
    ↓
run_cpp_to_lua_demo()                   （C++ 调 Lua）
    ↓
关闭 Lua 虚拟机
```

---

## 运行流程详解

### Lua 调用 C++

```lua
-- lua/lua_to_cpp/demo_call_cpp.lua
local mylib = require("mylib")
mylib.add(10, 20)
```

```
require("mylib")
  → 从 package.loaded 获取 C++ 注册的模块表
  → mylib.add(10, 20)
  → Lua 压栈参数，跳转 C++ 函数 l_add
  → C++ 计算结果压栈返回
```

### C++ 调用 Lua

```cpp
// src/cpp_to_lua/script_bridge.cpp
lua_getglobal(L, "lua_add");
lua_pushnumber(L, 3);
lua_pushnumber(L, 5);
lua_pcall(L, 2, 1, 0);
```

```
load_lua_callbacks → 执行 callbacks.lua，注册全局函数
run_cpp_to_lua_demo → lua_getglobal + lua_pcall 调用 Lua 函数
```

---

## 如何扩展

### 新增 C++ 函数供 Lua 调用

1. 在 `src/lua_to_cpp/mylib.cpp` 添加绑定函数并注册到 `mylib_funcs`
2. 在 `lua/lua_to_cpp/demo_call_cpp.lua` 中调用

无需修改 `main.cpp`。

### 新增 Lua 函数供 C++ 调用

1. 在 `lua/cpp_to_lua/callbacks.lua` 定义全局函数
2. 在 `src/cpp_to_lua/script_bridge.cpp` 的 `run_cpp_to_lua_demo()` 中添加调用逻辑

---

## 关键 API 说明

| API | 用途 |
|-----|------|
| `luaL_newstate()` | 创建 Lua 虚拟机 |
| `luaL_openlibs(L)` | 加载 Lua 标准库 |
| `luaL_requiref(L, name, openf, glb)` | 注册 C/C++ 模块 |
| `luaL_dofile(L, path)` | 加载并执行 Lua 脚本 |
| `lua_getglobal(L, name)` | 获取 Lua 全局函数 |
| `lua_pcall(L, nargs, nresults, errfunc)` | 保护模式调用 Lua 函数 |
| `luaopen_mylib(L)` | C++ 模块入口函数 |

---

## 常见问题

### 找不到 Lua 脚本

**原因**：未在项目根目录 `lua_cpp_demo` 下运行。

**解决**：使用 `./run.sh`，或 `cd lua_cpp_demo && ./build/main`。

### `require("mylib")` 报错

**原因**：C++ 侧未注册模块。

**解决**：确认 `main.cpp` 在加载脚本前调用了 `luaL_requiref(L, "mylib", luaopen_mylib, 1)`。

### 调用 Lua 函数失败

**原因**：未先加载 `callbacks.lua`，或函数名不一致。

**解决**：确保先调用 `load_lua_callbacks()`，且 C++ 中的函数名与 Lua 定义完全一致。

---

## 构建说明

`CMakeLists.txt` 编译产物：

| 目标 | 说明 |
|------|------|
| `libmylib.a` | Lua → C++ 绑定库 |
| `libscript_bridge.a` | C++ → Lua 桥接库 |
| `main` | 可执行文件（链接上述库 + Lua） |

采用静态库 + `luaL_requiref` 注册，无需额外 `.so` 文件。
