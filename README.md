# Lua + C++ 互调示例

本项目演示 **C++ 与 Lua 双向调用** 的最小完整流程，基于 [sol2](https://github.com/ThePhd/sol2) v3.3.1，代码按调用方向拆分目录：

- **Lua → C++**：`lua/lua_to_cpp/` + `src/lua_to_cpp/`
- **C++ → Lua**：`lua/cpp_to_lua/` + `src/cpp_to_lua/`

---

## 环境要求

| 依赖 | 说明 |
|------|------|
| C++ 编译器 | 支持 **C++17** 及以上（sol2 需要） |
| CMake | 3.14 及以上 |
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
[Step 1] Init sol::state + register mylib
[Step 2] Lua -> C++ basic + Phase 4a (table/usertype)
Lua calls C++ add(10, 20) = 30
Lua calls C++ hello('Lua') = Hello Lua
Lua calls C++ delete(10, 20) = -10
--- Phase 4a: Lua table -> C++ object ---
Created player: Archer hp=80 attack=15
After damage(20): Archer hp=60 attack=15
Stats table from C++: Archer 60 15 true
[Step 3] Load Lua callbacks module
Lua script loaded successfully!
[Step 4] C++ -> Lua basic callbacks
C++ calls Lua add(3,5) = 8
Hello World (from Lua)
C++ calls Lua delete(3,5) = -2
[Step 5] C++ -> Lua with C++ object (Phase 4b)
--- Phase 4b: C++ object -> Lua callback ---
C++ before Lua hit: Warrior hp=90 attack=10
Lua received C++ player: Warrior hp=90 attack=10 damage = 30
C++ after Lua hit: Warrior hp=60 attack=10, alive = true
```

---

## 项目结构

```
lua_cpp_demo/
├── include/
│   ├── lua_to_cpp/
│   │   ├── game_engine.h          # 纯 C++ 业务类 GameEngine
│   │   └── register_bindings.h    # sol2 绑定 + luaopen_mylib
│   └── cpp_to_lua/
│       └── script_bridge.h        # C++ 调用 Lua 的桥接接口
├── src/
│   ├── main.cpp                   # 五步编排（含阶段 4 演示）
│   ├── lua_to_cpp/
│   │   ├── game_engine.cpp        # GameEngine 实现
│   │   └── register_bindings.cpp  # sol2 绑定：函数 + usertype + table
│   └── cpp_to_lua/
│       └── script_bridge.cpp      # 加载回调、调用 Lua（含对象传参）
├── lua/
│   ├── lua_to_cpp/
│   │   └── demo_call_cpp.lua      # Lua 调用 C++ 示例脚本
│   └── cpp_to_lua/
│       └── callbacks.lua          # 供 C++ 调用的 Lua 回调（模块 + 全局兼容）
├── third_party/
│   └── sol2/                      # sol2 头文件库（v3.3.1，离线 vendored）
├── CMakeLists.txt
├── run.sh
└── README.md
```

---

## 功能说明

### 1. Lua → C++（`lua_to_cpp`）

| 文件 | 职责 |
|------|------|
| `src/lua_to_cpp/register_bindings.cpp` | sol2 绑定：自由函数 + `GameEngine` usertype + table 接口 |
| `src/lua_to_cpp/game_engine.cpp` | 纯 C++ 业务逻辑，不依赖 Lua |
| `lua/lua_to_cpp/demo_call_cpp.lua` | 基础调用 + **阶段 4a** table/usertype 演示 |

| Lua 调用 | C++ 实现 | 功能 |
|----------|----------|------|
| `mylib.add(a, b)` | sol2 lambda | 返回 `a + b` |
| `mylib.delete(a, b)` | sol2 lambda | 返回 `a - b` |
| `mylib.hello(name)` | sol2 lambda | 返回 `"Hello {name}"` |
| `mylib.create_from_config(table)` | `config_from_table` → `GameEngine` | **Lua table → C++ 对象** |
| `mylib.get_player_stats(player)` | `stats_to_table` | **C++ 对象 → Lua table** |
| `player:damage(n)` | usertype 成员函数 | Lua 修改 C++ 对象状态 |

### 2. C++ → Lua（`cpp_to_lua`）

| 文件 | 职责 |
|------|------|
| `lua/cpp_to_lua/callbacks.lua` | 模块 `M` 定义回调；全局 `lua_*` 函数兼容旧调用 |
| `src/cpp_to_lua/script_bridge.cpp` | 加载脚本、存入 `cpp_callbacks`、调用 Lua 函数 |

| C++ 调用 | Lua 函数 | 功能 |
|----------|----------|------|
| `run_cpp_to_lua_call()` 内 | `lua_add(3, 5)` | 返回 `8` |
| 同上 | `lua_hello("World")` | 打印问候语 |
| 同上 | `lua_delete(3, 5)` | 返回 `-2` |
| `run_phase4_object_callback()` | `cpp_callbacks.on_player_hit(player, dmg)` | **阶段 4b**：Lua 接收 C++ 对象并修改 |

### 3. 主程序五步编排（`main.cpp`）

```
[Step 1] sol::state + luaL_requiref 注册 mylib
[Step 2] demo_call_cpp.lua（基础 + 阶段 4a table/usertype）
[Step 3] load_lua_callbacks(callbacks.lua) → 存入 cpp_callbacks
[Step 4] run_cpp_to_lua_call（基础回调）
[Step 5] run_phase4_object_callback（阶段 4b 对象传入 Lua）
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
  → luaopen_mylib → register_cpp_bindings 创建 module 表
  → 写入 package.loaded["mylib"]
  → mylib.add(10, 20)
  → sol2 调用 C++ lambda，返回值自动压栈
```

### C++ 调用 Lua

```cpp
// src/cpp_to_lua/script_bridge.cpp
sol::protected_function fn = lua["lua_add"];
sol::protected_function_result ret = fn(3, 5);
double result = ret.get<double>();
```

```
load_lua_callbacks
  → script_file(callbacks.lua) 得到模块表 M
  → lua["cpp_callbacks"] = M
run_cpp_to_lua_call
  → lua["lua_add"](3, 5)  （全局兼容函数）
run_phase4_object_callback
  → lua["cpp_callbacks"]["on_player_hit"](player, 30)
```

---

## 阶段 4：table 与 C++ 对象（sol2 usertype）

| 子阶段 | 方向 | 实现 | 运行体现 |
|--------|------|------|----------|
| **4a** | Lua table → C++ | `create_from_config({name, hp, attack})` | `Created player: Archer...` |
| **4a** | C++ → Lua table | `get_player_stats(player)` | `Stats table from C++: ...` |
| **4b** | C++ 对象 → Lua | `on_player_hit(player, 30)` | `Lua received C++ player: ...` |

### 4a：Lua table → C++ 对象（Lua 造 table，C++ 读）

```lua
-- demo_call_cpp.lua：table 由 Lua 侧字面量创建
local player = mylib.create_from_config({ name = "Archer", hp = 80, attack = 15 })
player:damage(20)
```

```cpp
// register_bindings.cpp：从 sol::table 读出字段，构造 GameEngine
PlayerConfig config_from_table(const sol::table& config) {
    PlayerConfig cfg;
    cfg.name = config.get_or<std::string>("name", "unknown");
    cfg.hp = config.get_or("hp", static_cast<int>(100));
    cfg.attack = config.get_or("attack", static_cast<int>(10));
    return cfg;
}
module["create_from_config"] = [](const sol::table& config) {
    return GameEngine(config_from_table(config));
};
```

### 4a：C++ 对象 → Lua table（C++ 主动造 table）

```lua
local stats = mylib.get_player_stats(player)
print(stats.name, stats.hp, stats.attack, stats.alive)
```

```cpp
// register_bindings.cpp：C++ 在 Lua VM 里 create_table 并填字段
sol::table stats_to_table(sol::state_view lua, const GameEngine& player) {
    sol::table stats = lua.create_table();   // 等价于 Lua 的 stats = {}
    stats["name"] = player.name();
    stats["hp"] = player.hp();
    stats["attack"] = player.attack();
    stats["alive"] = player.is_alive();
    return stats;                            // sol2 把 table 作为返回值交给 Lua
}

// lambda 捕获 lua：注册时保存 VM 上下文，Lua 稍后调用时仍能 create_table
module["get_player_stats"] = [lua](const GameEngine& player) {
    return stats_to_table(lua, player);
};
```

> **为何 `[lua]` 捕获？** `get_player_stats` 被 Lua 延迟调用时，外层 `register_cpp_bindings` 的 `lua` 参数已出栈；`stats_to_table` 又必须在某个 `lua_State*` 上执行 `create_table()`。`sol::state_view` 是非拥有的轻量包装，值捕获 `[lua]` 后闭包内仍指向同一 VM。

### 4b：C++ 将对象传入 Lua 回调

```cpp
// main.cpp
GameEngine warrior = GameEngine::create_player("Warrior");
warrior.damage(10);
run_phase4_object_callback(lua, warrior);

// script_bridge.cpp
sol::table callbacks = lua["cpp_callbacks"];
sol::protected_function on_hit = callbacks["on_player_hit"];
on_hit(player, 30);
```

```lua
-- callbacks.lua
function M.on_player_hit(player, damage)
    player:damage(damage)   -- player 是 C++ GameEngine 的 usertype
    return player:is_alive()
end
```

---

## sol2 迁移说明（阶段 2）

项目已从原生 Lua C API 迁移至 sol2，主要变更：

| 文件 | 改动 |
|------|------|
| `CMakeLists.txt` | 引入 `third_party/sol2`，启用 C++17 |
| `src/lua_to_cpp/register_bindings.cpp` | 替代原 `mylib.cpp`：`luaL_Reg` → `sol::table` + lambda + usertype |
| `src/cpp_to_lua/script_bridge.cpp` | `lua_pcall` → `sol::protected_function` |
| `src/main.cpp` | `lua_State*` → `sol::state` |

**Lua → C++ 绑定对比**

```cpp
// 迁移前
static int l_add(lua_State* L) {
    lua_Integer a = luaL_checkinteger(L, 1);
    lua_Integer b = luaL_checkinteger(L, 2);
    lua_pushinteger(L, a + b);
    return 1;
}

// 迁移后
module["add"] = [](int a, int b) { return a + b; };
```

**C++ → Lua 调用对比**

```cpp
// 迁移前
lua_getglobal(L, "lua_add");
lua_pushnumber(L, 3);
lua_pushnumber(L, 5);
lua_pcall(L, 2, 1, 0);

// 迁移后
sol::protected_function fn = lua["lua_add"];
sol::protected_function_result ret = fn(3, 5);
```

> **注意**：C++ 关键字 `delete` 不能用于 `create_table_with` 宏，需改用 `module["delete"] = ...` 单独注册。

---

## 如何扩展

### 新增 C++ 函数供 Lua 调用

1. 在 `src/lua_to_cpp/register_bindings.cpp` 的 `register_cpp_bindings` 中添加绑定
2. 在 `lua/lua_to_cpp/demo_call_cpp.lua` 中调用

若新函数需要在 C++ 里**创建 Lua table 并返回**，lambda 需捕获 `lua`（与 `get_player_stats` 相同模式）。

无需修改 `main.cpp`。

### 新增 Lua 函数供 C++ 调用

1. 在 `lua/cpp_to_lua/callbacks.lua` 的模块 `M` 中定义函数（可选：加全局 `lua_*` 兼容）
2. 在 `src/cpp_to_lua/script_bridge.cpp` 中添加 sol2 调用

---

## 关键 API 说明

### sol2（当前使用）

| API | 用途 |
|-----|------|
| `sol::state` | 创建并管理 Lua 虚拟机 |
| `sol::state_view` | 非拥有型 Lua 状态视图（可安全值捕获进 lambda） |
| `lua.create_table()` | 在 C++ 侧创建 Lua table |
| `lua.new_usertype<T>(...)` | 注册 C++ 类，Lua 侧可 `:method()` 调用 |
| `lua.script_file(path)` | 加载并执行 Lua 脚本 |
| `lua["func"](args...)` | 调用 Lua 全局函数 |
| `sol::protected_function` | 带错误捕获的函数调用 |
| `config.get_or<K>(key, default)` | 从 Lua table 读字段并带默认值 |

### 原生 Lua C API（仍保留部分）

| API | 用途 |
|-----|------|
| `luaL_requiref(L, name, openf, glb)` | 注册 C/C++ 模块（`main.cpp` 中注册 mylib） |
| `luaopen_mylib(L)` | C++ 模块入口（`extern "C"`，供 `require` 使用） |

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

**解决**：确保先调用 `load_lua_callbacks()`；阶段 4b 使用 `cpp_callbacks.on_player_hit`，阶段 1–3 使用全局 `lua_add` 等。

### `get_player_stats` 编译报错「lua 未定义」

**原因**：lambda 未捕获 `lua`，无法在闭包内调用 `stats_to_table`。

**解决**：使用 `[lua](const GameEngine& player) { ... }` 值捕获 `sol::state_view`。

---

## 构建说明

`CMakeLists.txt` 编译产物：

| 目标 | 说明 |
|------|------|
| `liblua_to_cpp.a` | GameEngine + sol2 绑定 |
| `libscript_bridge.a` | C++ → Lua 桥接库（sol2） |
| `main` | 可执行文件（链接上述库 + Lua） |

sol2 以头文件库形式 vendored 在 `third_party/sol2`，通过 `sol2::sol2` 目标链接，无需联网 FetchContent，也无需额外编译 sol2 源文件。
