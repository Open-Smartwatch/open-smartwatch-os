/*
** $Id: linit.c,v 1.39.1.1 2017/04/19 17:20:42 roberto Exp $
** Initialization of libraries for lua.c and other clients
** See Copyright Notice in lua.h
*/

#ifdef OSW_FEATURE_LUA

#include "Arduino.h"

#define linit_c
#define LUA_LIB

/*
** If you embed Lua in your program and need to open the standard
** libraries, call luaL_openlibs in your program. If you need a
** different set of libraries, copy this file to your project and edit
** it to suit your needs.
**
** You can also *preload* libraries, so that a later 'require' can
** open the library, which is already linked to the application.
** For that, do the following code:
**
**  luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_PRELOAD_TABLE);
**  lua_pushcfunction(L, luaopen_modname);
**  lua_setfield(L, -2, modname);
**  lua_pop(L, 1);  // remove PRELOAD table
*/

#include "lprefix.h"


#include <stddef.h>

extern "C" {
#include "lua.h"

#include "lualib.h"
#include "lauxlib.h"

    extern int luaopen_osw(lua_State* L);
}

/*
** these libs are loaded by lua.c and are readily available to any Lua
** program
*/
static const luaL_Reg loadedlibs[] = {
    {"_G", luaopen_base},
    {LUA_LOADLIBNAME, luaopen_package},
    {LUA_COLIBNAME, luaopen_coroutine},
    {LUA_TABLIBNAME, luaopen_table},
    {LUA_IOLIBNAME, luaopen_io},
//   {LUA_OSLIBNAME, luaopen_os},
    {LUA_STRLIBNAME, luaopen_string},
    {LUA_MATHLIBNAME, luaopen_math},
    {LUA_UTF8LIBNAME, luaopen_utf8},
    {LUA_DBLIBNAME, luaopen_debug},
    {"osw", luaopen_osw},
#if defined(LUA_COMPAT_BITLIB)
    {LUA_BITLIBNAME, luaopen_bit32},
#endif
    {NULL, NULL}
};

/*
LUALIB_API void luaL_openlibs (lua_State* L) {
    const luaL_Reg* lib;
    // "require" functions from 'loadedlibs' and set results to global table 
    for (lib = loadedlibs; lib->func; lib++) {
        luaL_requiref(L, lib->name, lib->func, 1);
        lua_pop(L, 1);  // remove lib 
    }
}
*/

void printLuaStack(lua_State *L) {
    int top = lua_gettop(L);

    Serial.println("From top to bottom, the lua stack is");
    for (unsigned index = top; index > 0; index--)
    {
        int type = lua_type(L, index);
        switch (type)
        {
            // booleans
            case LUA_TBOOLEAN:
                Serial.printf("%s\n", lua_toboolean(L, index) ? "true" : "false");
                break;

            // numbers
            case LUA_TNUMBER:
                Serial.printf("%f\n", lua_tonumber(L, index));
                break;

        // strings
            case LUA_TSTRING:
                Serial.printf("%s\n", lua_tostring(L, index));
                break;

            // other
            default:
                Serial.printf("%s\n", lua_typename(L, type));
                break;
        }
    }
}
#endif
