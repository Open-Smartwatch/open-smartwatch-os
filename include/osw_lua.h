#pragma once

#ifdef OSW_FEATURE_LUA
extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
};

void halToLua(lua_State *L);
#endif