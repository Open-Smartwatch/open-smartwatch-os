#include <Arduino.h>
#include <Wire.h>
#include <config.h>
#include <osw_app.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <osw_pins.h>
#include <osw_ui.h>
#include <osw_ulp.h>
#include <rom/rtc.h>
#include <stdlib.h>  //randomSeed
#include <time.h>    //time

#ifdef RAW_SCREEN_SERVER
#include <osw_screenserver.h>
#endif

#ifdef OSW_FEATURE_WIFI
#ifndef CONFIG_WIFI_SSID
#pragma error "!!!!!!!!"
#pragma error "PLEASE COPY include/config.h.example TO include/config.h"
#pragma error "AND CONFIGURE THE DEFINES FOR YOUR WATCH"
#pragma error "!!!!!!!!"
#endif
#endif

// #include "./apps/_experiments/runtime_test.h"
#include "./apps/_experiments/hello_world.h"
#ifdef OSW_FEATURE_LUA
#include "./apps/main/luaapp.h"
#endif
#include "./apps/games/snake_game.h"
#ifdef OSW_FEATURE_WIFI
#include "./apps/main/OswAppWebserver.h"
#endif
#include "./apps/main/stopwatch.h"
#include "./apps/main/switcher.h"
#include "./apps/tools/button_test.h"
#include "./apps/tools/print_debug.h"
#include "./apps/tools/time_config.h"
#include "./apps/tools/water_level.h"
#include "./apps/watchfaces/watchface.h"
#include "./apps/watchfaces/watchface_binary.h"
#include "./apps/watchfaces/watchface_digital.h"
#include "./overlays/overlays.h"
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
#include "./apps/_experiments/magnetometer_calibrate.h"
#include "./apps/main/map.h"
#endif
#include "./services/OswServiceManager.h"
#include "./services/OswServiceTaskBLECompanion.h"
#include "debug_scani2c.h"
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
#include "hal/esp32/sd_filesystem.h"
#else
#include "hal/esp32/spiffs_filesystem.h"
#endif
#include "services/OswServiceTaskMemMonitor.h"
#include "services/OswServiceTasks.h"

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
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
  hal->setupAccelerometer();
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

  randomSeed(hal->getUTCTime());  // Make sure the RTC is loaded and get the real time (!= 0, other than time(nullptr),
                                  // which is possibly 0 right now)

  mainAppSwitcher->setup(hal);

#ifdef DEBUG
  Serial.println("Setup Done");
#endif
#ifdef RAW_SCREEN_SERVER
  screenserver_setup(hal);
#endif

#if USE_ULP == 1
  // register the ULP program
  init_ulp();
#endif
}

void loop() {
  static long lastFlush = 0;
  static boolean delayedAppInit = true;

// check possible interaction with ULP program
#if USE_ULP == 1
  loop_ulp();
#endif

  hal->handleWakeupFromLightSleep();
  hal->checkButtons();
  hal->updateRtc();
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
    if (delayedAppInit) {
      // fix flickering display on latest Arduino_GFX library
      ledcWrite(1, OswConfigAllKeys::settingDisplayBrightness.get());
    }
    lastFlush = millis();
  }

#ifdef RAW_SCREEN_SERVER
  screenserver_loop(hal);
#endif

  if (delayedAppInit) {
    delayedAppInit = false;
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
    mainAppSwitcher->registerApp(new OswAppMap());
    mainAppSwitcher->registerApp(new OswAppPrintDebug());
    mainAppSwitcher->registerApp(new OswAppMagnetometerCalibrate());
#endif
    // For a short howto write your own apps see: app below
    // mainAppSwitcher->registerApp(new OswAppHelloWorld());

    // tools

#if TOOL_STOPWATCH == 1
    mainAppSwitcher->registerApp(new OswAppStopWatch());
#endif
#if TOOL_WATERLEVEL == 1
    mainAppSwitcher->registerApp(new OswAppWaterLevel());
#endif

// config
#ifdef OSW_FEATURE_WIFI
    settingsAppSwitcher->registerApp(new OswAppWebserver());
#endif

    settingsAppSwitcher->registerApp(new OswAppTimeConfig());
    settingsAppSwitcher->paginationEnable();
    mainAppSwitcher->registerApp(settingsAppSwitcher);

    // games

#if GAME_SNAKE == 1
    mainAppSwitcher->registerApp(new OswAppSnakeGame());
#endif

#ifdef OSW_FEATURE_LUA
    mainAppSwitcher->registerApp(new OswLuaApp("stopwatch.lua"));
#endif
  }

#ifdef DEBUG
  OswServiceAllTasks::memory.updateLoopTaskStats();
#endif
}
