cmake_minimum_required(VERSION 3.14)
project(lua_cpp_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Lua REQUIRED)
include_directories(${LUA_INCLUDE_DIR} include)

add_subdirectory(third_party/sol2 EXCLUDE_FROM_ALL)

add_library(lua_to_cpp STATIC
    src/lua_to_cpp/game_engine.cpp
    src/lua_to_cpp/register_bindings.cpp)
target_link_libraries(lua_to_cpp PUBLIC sol2::sol2)

add_library(script_bridge STATIC src/cpp_to_lua/script_bridge.cpp)
target_link_libraries(script_bridge PUBLIC sol2::sol2 lua_to_cpp)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE lua_to_cpp script_bridge ${LUA_LIBRARIES})
