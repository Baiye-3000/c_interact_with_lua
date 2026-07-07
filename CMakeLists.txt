cmake_minimum_required(VERSION 3.10)
project(lua_cpp_demo)

find_package(Lua REQUIRED)
include_directories(${LUA_INCLUDE_DIR} include)

add_library(mylib STATIC src/lua_to_cpp/mylib.cpp)
add_library(script_bridge STATIC src/cpp_to_lua/script_bridge.cpp)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE mylib script_bridge ${LUA_LIBRARIES})
