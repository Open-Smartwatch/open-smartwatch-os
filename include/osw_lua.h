#pragma once

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
};

void halToLua(lua_State *L, OswHal *hal);