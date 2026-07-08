#ifndef SCRIPT_BRIDGE_H
#define SCRIPT_BRIDGE_H

#include <sol/sol.hpp>

#include "lua_to_cpp/game_engine.h"

bool load_lua_callbacks(sol::state_view lua, const char* path);
bool run_cpp_to_lua_call(sol::state_view lua);
bool run_phase4_object_callback(sol::state_view lua, GameEngine& player);

#endif // SCRIPT_BRIDGE_H
