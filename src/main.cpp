#include <Arduino.h>
#include <Wire.h>
#include <config.h>
#include <osw_app.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <osw_pins.h>
#include <osw_ui.h>
#include <rom/rtc.h>
#include <stdlib.h>  //randomSeed
#include <time.h>    //time
#ifdef RAW_SCREEN_SERVER
#include <osw_screenserver.h>
#endif

#ifndef CONFIG_WIFI_SSID
#pragma error "!!!!!!!!"
#pragma error "PLEASE COPY include/config.h.example TO include/config.h"
#pragma error "AND CONFIGURE THE DEFINES FOR YOUR WATCH"
#pragma error "!!!!!!!!"
#endif

// #include "./apps/_experiments/runtime_test.h"
#include "./apps/_experiments/hello_world.h"
#ifdef LUA_SCRIPTS
#include "./apps/main/luaapp.h"
#endif
#include "./apps/games/snake_game.h"
#include "./apps/main/OswAppWebserver.h"
#include "./apps/main/stopwatch.h"
#include "./apps/main/switcher.h"
#include "./apps/tools/button_test.h"
#include "./apps/tools/print_debug.h"
#include "./apps/tools/time_config.h"
#include "./apps/tools/water_level.h"
#include "./apps/watchfaces/watchface.h"
#include "./apps/watchfaces/watchface_binary.h"
#include "./apps/watchfaces/watchface_digital.h"
#include "./apps/examples/fonts/fonts_example.h"
#include "./overlays/overlays.h"
#if defined(GPS_EDITION)
#include "./apps/main/map.h"
#include "./apps/_experiments/compass_calibrate.h"
#endif
#include "./services/OswServiceManager.h"
#include "./services/OswServiceTaskBLECompanion.h"
#include "debug_scani2c.h"
#if defined(GPS_EDITION)
#include "hal/esp32/sd_filesystem.h"
#else
#include "hal/esp32/spiffs_filesystem.h"
#endif
#include "services/OswServiceTaskMemMonitor.h"
#include "services/OswServiceTasks.h"

#if defined(GPS_EDITION)
OswHal *hal = new OswHal(new SDFileSystemHal());
#else
OswHal *hal = new OswHal(new SPIFFSFileSystemHal());
#endif
// OswAppRuntimeTest *runtimeTest = new OswAppRuntimeTest();

uint16_t mainAppIndex = 0;  // -> wakeup from deep sleep returns to watch face (and allows auto sleep)
RTC_DATA_ATTR uint16_t watchFaceIndex = 0;
uint16_t settingsAppIndex = 0;

OswAppSwitcher *mainAppSwitcher = new OswAppSwitcher(BUTTON_1, LONG_PRESS, true, true, &mainAppIndex);
OswAppSwitcher *watchFaceSwitcher = new OswAppSwitcher(BUTTON_1, SHORT_PRESS, false, false, &watchFaceIndex);
OswAppSwitcher *settingsAppSwitcher = new OswAppSwitcher(BUTTON_1, SHORT_PRESS, false, false, &settingsAppIndex);

void setup() {
  Serial.begin(115200);

  // Load config as early as possible, to ensure everyone can access it.
  OswConfig::getInstance()->setup();

  // First setup hardware/sensors/display -> might be used by background services
  hal->setupPower();
  hal->setupButtons();
  hal->setupSensors();
  hal->setupTime();
  hal->setupDisplay();
  hal->setupFileSystem();

  OswUI::getInstance()->setup(hal);

  // Fire off the service manager
  OswServiceManager::getInstance().setup(hal);

  watchFaceSwitcher->registerApp(new OswAppWatchface());
  watchFaceSwitcher->registerApp(new OswAppWatchfaceDigital());
  watchFaceSwitcher->registerApp(new OswAppWatchfaceBinary());
  mainAppSwitcher->registerApp(watchFaceSwitcher);

  randomSeed(hal->getUTCTime()); // Make sure the RTC is loaded and get the real time (!= 0, other than time(nullptr), which is possibly 0 right now)

  mainAppSwitcher->setup(hal);

#ifdef DEBUG
  Serial.println("Setup Done");
#endif
#ifdef RAW_SCREEN_SERVER
  screenserver_setup(hal);
#endif
}

void loop() {
  static long lastFlush = 0;
  static boolean delayedAppInit = true;

  hal->handleWakeupFromLightSleep();
  hal->checkButtons();
  hal->updateAccelerometer();

  mainAppSwitcher->loop(hal);

  // limit to 30 fps and handle display flushing
  if (millis() - lastFlush > 1000 / 30 && hal->isRequestFlush()) {
    // only draw overlays if enabled
    if (OswConfigAllKeys::settingDisplayOverlays.get()) {
      // only draw on first face if enabled, or on all others
      if ((mainAppIndex == 0 && OswConfigAllKeys::settingDisplayOverlaysOnWatchScreen.get()) || mainAppIndex != 0) {
        drawOverlays(hal);
      }
    }
    hal->flushCanvas();
    lastFlush = millis();
  }

#ifdef RAW_SCREEN_SERVER
  screenserver_loop(hal);
#endif

  if (delayedAppInit) {
    delayedAppInit = false;
#ifdef GPS_EDITION
    mainAppSwitcher->registerApp(new OswAppMap());
    mainAppSwitcher->registerApp(new OswAppPrintDebug());
    mainAppSwitcher->registerApp(new OswAppCompassCalibrate());
#endif
    // enable / sort your apps here:
    // tests
    // mainAppSwitcher->registerApp(new OswAppHelloWorld());
    // games
    // mainAppSwitcher->registerApp(new OswAppSnakeGame());
    // tools
    mainAppSwitcher->registerApp(new OswAppStopWatch());
    mainAppSwitcher->registerApp(new OswAppWaterLevel());
    // code examples
    // mainAppSwitcher->registerApp(new OswAppFontsExample());
#ifdef LUA_SCRIPTS
    mainAppSwitcher->registerApp(new OswLuaApp("stopwatch.lua"));
#endif
    // config
    settingsAppSwitcher->registerApp(new OswAppWebserver());
    settingsAppSwitcher->registerApp(new OswAppTimeConfig());
    mainAppSwitcher->registerApp(settingsAppSwitcher);
  }

#ifdef DEBUG
  OswServiceAllTasks::memory.updateLoopTaskStats();
#endif
}
