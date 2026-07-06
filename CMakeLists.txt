cmake_minimum_required(VERSION 3.10)
project(lua_cpp_demo)

find_package(Lua REQUIRED)
include_directories(${LUA_INCLUDE_DIR} include)

# C++ 封装为静态库，由 main 链接并注册到 Lua
add_library(mylib STATIC src/mylib.cpp)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE mylib ${LUA_LIBRARIES})
