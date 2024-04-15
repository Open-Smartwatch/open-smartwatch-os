#ifdef OSW_FEATURE_LUA
#include <osw_hal.h>
#include <osw_lua.h>

#include <OswAppV1.h>

#define LUA_SETUP_FUNC "setup"
#define LUA_LOOP_FUNC "loop"
#define LUA_STOP_FUNC "stop"

#ifdef OSW_EMULATOR
#define LUA_PATH "../data/lua/"
#else
#define LUA_PATH FS_MOUNT_POINT "/lua/"
#endif

#define LUA_APP_PATH LUA_PATH "apps/"
#define LUA_LIB_PATH LUA_PATH "libs/"

#define LUA_APP_SEARCH_PATH LUA_APP_PATH "?.lua"
#define LUA_LIB_SEARCH_PATH LUA_LIB_PATH "?.lua"
#define LUA_PACKAGE_CMD "package.path = package.path .. ';" LUA_APP_SEARCH_PATH ";" LUA_LIB_SEARCH_PATH "'"

class OswLuaApp : public OswApp {
  public:
    OswLuaApp(const char* file) : file(file) {};
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswLuaApp() {};

  private:
    void cleanupState();
    void printLuaError();

    lua_State* luaState = NULL;

    const char* file;
};
#endif
