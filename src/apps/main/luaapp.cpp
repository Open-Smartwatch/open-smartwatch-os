#ifdef OSW_FEATURE_LUA
#include "./apps/main/luaapp.h"

#include <osw_app.h>
#include <osw_hal.h>
#include <string>

void OswLuaApp::setup() {
    luaState = luaL_newstate();

    if (luaState) {
        luaL_openlibs(luaState);
        halToLua(luaState);

        //Include search paths
        luaL_dostring(luaState, LUA_PACKAGE_CMD);

        std::string filePath = (std::string(LUA_APP_PATH) + std::string(file));
        if (luaL_dofile(luaState, filePath.c_str())) {
            printLuaError();
            cleanupState();
            return;
        }

        lua_getglobal(luaState, LUA_SETUP_FUNC);
        if (lua_pcall(luaState, 0, 0, 0)) {
            printLuaError();
        }
    }
}

void OswLuaApp::loop() {
    if (luaState) {
        lua_getglobal(luaState, LUA_LOOP_FUNC);
        if (lua_pcall(luaState, 0, 0, 0)) {
            printLuaError();
        }

        //Force GC to run after loop
        lua_gc(luaState, LUA_GCCOLLECT, 0);
    }
}

void OswLuaApp::stop() {
    if (luaState) {
        lua_getglobal(luaState, LUA_STOP_FUNC);
        if (lua_pcall(luaState, 0, 0, 0)) {
            printLuaError();
        }
    }

    cleanupState();
}

void OswLuaApp::cleanupState() {
    if (luaState) {
        lua_close(luaState);
        luaState = NULL;
    }
}

void OswLuaApp::printLuaError() {
    Serial.println(lua_tostring(luaState, -1)); 
}
#endif