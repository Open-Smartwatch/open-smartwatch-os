
#include "./osw_defaultlauncher.h"

#include "./apps/main/map.h"
#include <osw_app.h>
#include <osw_hal.h>



// #include "./apps/_experiments/runtime_test.h"
#include "./apps/_experiments/hello_world.h"
//#include "./apps/_experiments/menu.h"
#include "./apps/main/luaapp.h"
#include "./apps/main/stopwatch.h"
#include "./apps/main/watchface.h"
#include "./apps/tools/print_debug.h"
#include "./apps/tools/time_from_web.h"
#include "./apps/tools/water_level.h"
#include "./overlays/overlays.h"
#include "apps/lua/mylua_example.h"


#define DEFAULTLAUNCHER_SWITCH_TIMEOUT 500
#define DEFAULTLAUNCHER_SLEEP_TIMEOUT 1500

// HINT: NUM_APPS must match the number of apps below!

#if defined(GPS_EDITION)
#define NUM_APPS 5
#else
#define NUM_APPS 4
#endif

RTC_DATA_ATTR uint8_t appPtr = 0;

OswApp *mainApps[] = {
    new OswAppWatchface(),
//    new OswAppMenu(),
// new OswAppHelloWorld(),
#if defined(GPS_EDITION)
    new OswAppMap(),
#endif
    // new OswAppPrintDebug(),
    new OswAppStopWatch(),    //
    new OswAppTimeFromWeb(),  //
    new OswAppWaterLevel()
    // new OswLuaApp(myLuaExample)
};

void OswDefaultLauncher::setup(OswHal* hal) {
  _hal = hal;
  mainApps[appPtr]->setup(_hal);
}

void OswDefaultLauncher::loop() {
    
  static long lastFlush = 0;
  static unsigned long appOnScreenSince = millis();
  static long lastBtn1Duration = 0;

    
  if (_hal->btn1Down()) {
    lastBtn1Duration = _hal->btn1Down();
  }

  // handle long click to sleep
  if (!_hal->btn1Down() && lastBtn1Duration >= DEFAULTLAUNCHER_SLEEP_TIMEOUT) {
    _hal->getCanvas()->getGraphics2D()->fill(rgb565(0, 0, 0));
    _hal->flushCanvas();
    _hal->deepSleep();
  }

  // handle medium click to switch
  if (!_hal->btn1Down() && lastBtn1Duration >= DEFAULTLAUNCHER_SWITCH_TIMEOUT) {
    // switch app
    mainApps[appPtr]->stop(_hal);
    appPtr++;
    appPtr %= NUM_APPS;
    mainApps[appPtr]->setup(_hal);
    appOnScreenSince = millis();

    lastBtn1Duration = 0;
  }

  mainApps[appPtr]->loop(_hal);

  
  // limit to 30 fps and handle display flushing
  if (millis() - lastFlush > 1000 / 30 && _hal->isRequestFlush()) {
    drawOverlays(_hal);
    _hal->flushCanvas();
    lastFlush = millis();
  }

  // auto sleep on first screen
  if (appPtr == 0 && (millis() - appOnScreenSince) > 5000) {
    _hal->gfx()->fill(rgb565(0, 0, 0));
    _hal->flushCanvas();
    _hal->deepSleep();
  }
}

void OswDefaultLauncher::stop() {

}