#pragma once

#ifdef OSW_FEATURE_LUA
#include <osw_hal.h>
#include <osw_lua.h>

#include "osw_app.h"

#define LUA_SETUP_FUNC "setup"
#define LUA_LOOP_FUNC "loop"
#define LUA_STOP_FUNC "stop"

#define LUA_APP_PATH FS_MOUNT_POINT "/lua/apps/"
#define lUA_APP_SEARCH_PATH FS_MOUNT_POINT LUA_APP_PATH "?.lua"
#define lUA_LIB_SEARCH_PATH FS_MOUNT_POINT "/lua/libs/?.lua"
#define LUA_PACKAGE_CMD "package.path = package.path .. ';" lUA_APP_SEARCH_PATH ";" lUA_LIB_SEARCH_PATH "'"

class OswLuaApp : public OswApp {
 public:
  OswLuaApp(const char* file) : file(file){};
  virtual void setup() override;
  virtual void loop() override;
  virtual void stop() override;
  ~OswLuaApp(){};

 private:
  void cleanupState();
  void printLuaError();

  lua_State* luaState = NULL;

  const char* file;
};
#endif