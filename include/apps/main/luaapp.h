#pragma once

#include <osw_hal.h>
#include <osw_lua.h>
#include "osw_app.h"

#define LUA_SETUP_FUNC "setup"
#define LUA_LOOP_FUNC "loop"
#define LUA_STOP_FUNC "stop"

class OswLuaApp : public OswApp {
 public:
  OswLuaApp(const char *luaStr): fileStr(luaStr) {};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswLuaApp(){};

 private:
    void cleanupState();
    void printLuaError();

    lua_State *luaState = NULL;

    const char *fileStr;
};