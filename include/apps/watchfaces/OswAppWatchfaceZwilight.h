#include <osw_hal.h>
#include <osw_ui.h>
#include <OswAppV2.h>

#ifdef OSW_FEATURE_LUA
#include <osw_lua.h>
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
#endif

enum SUNTIMES 
{
    SUNTIME_ERROR = 0, 
    LAST_MIDNIGHT,
    ASTRONOMIC_DAWN,
    NAUTIC_DAWN,
    CIVIL_DAWN,
    SUN_RISE,
    NOON,
    SUN_SET,
    CIVIL_DUSK,
    NAUTIC_DUSK,
    ASTRONOMIC_DUSK,
    MIDNIGHT
};

class OswAppWatchfaceZwilight : public OswAppV2 {
  public:
    constexpr static const char* APP_ID = "osw.wf.zwilight";
    const char* getAppId() override;
    const char* getAppName() override;

    void onStart() override;
    void onLoop() override;
    void onDraw() override;
    void onStop() override;
    void onButton(Button id, bool up, ButtonStateNames state) override;

    OswAppWatchfaceZwilight() {
      lastTime = 0;
      #ifdef OSW_FEATURE_LUA
      luaState = nullptr;
      #endif
    }

    ~OswAppWatchfaceZwilight() {
      cleanupLuaState();
    }

  private:
    time_t lastTime = 0;

    void displayWeekDay2(const char* weekday);
    void drawWatch(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second);

    bool getTwilights();
    void cleanupLuaState();
    void printLuaError();

    float sunTimes[12];
    #ifdef OSW_FEATURE_LUA
    lua_State* luaState = nullptr;
    #endif
};
