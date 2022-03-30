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
#include "./apps/games/brick_breaker.h"
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
#include "./apps/watchfaces/watchface_mix.h"
#if OSW_PLATFORM_ENVIRONMENT_MAGNETOMETER == 1 && OSW_PLATFORM_HARDWARE_QMC5883L == 1
#include "./apps/_experiments/magnetometer_calibrate.h"
#endif
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
#include "./apps/main/map.h"
#endif
#include "./services/OswServiceTaskBLECompanion.h"
#include "debug_scani2c.h"
#include "services/OswServiceTaskMemMonitor.h"
#include "services/OswServiceTasks.h"
#ifdef OSW_FEATURE_WIFI
#include <services/OswServiceTaskWiFi.h>
#endif

OswHal* hal = nullptr;
// OswAppRuntimeTest *runtimeTest = new OswAppRuntimeTest();

uint16_t mainAppIndex = 0;              // -> wakeup from deep sleep returns to watch face (and allows auto sleep)
RTC_DATA_ATTR uint16_t watchFaceIndex;  // Will only be initialized after deep sleep inside the setup() ↓
uint16_t settingsAppIndex = 0;

OswAppSwitcher mainAppSwitcher(BUTTON_1, LONG_PRESS, true, true, &mainAppIndex);
OswAppSwitcher watchFaceSwitcher(BUTTON_1, SHORT_PRESS, false, false, &watchFaceIndex);
OswAppSwitcher settingsAppSwitcher(BUTTON_1, SHORT_PRESS, false, false, &settingsAppIndex);

void setup() {
  Serial.begin(115200);
  Serial.println(String("Welcome to the OSW-OS! This build is based on commit ") + GIT_COMMIT_HASH +" from " + GIT_BRANCH_NAME +
    ". Compiled at " + __DATE__ + " " + __TIME__ + " for platform " + PIO_ENV_NAME + ".");

  hal = OswHal::getInstance();

  // Load config as early as possible, to ensure everyone can access it.
  OswConfig::getInstance()->setup();
  watchFaceIndex = OswConfigAllKeys::settingDisplayDefaultWatchface.get().toInt();

  // First setup hardware/sensors/display -> might be used by background services
  try {
    hal->setup(false);
  } catch(const std::runtime_error& e) {
    Serial.println(String("CRITICAL ERROR AT BOOTUP: ") + e.what());
    sleep(10);
    ESP.restart();
  }

  watchFaceSwitcher.registerApp(new OswAppWatchface());
  watchFaceSwitcher.registerApp(new OswAppWatchfaceDigital());
  watchFaceSwitcher.registerApp(new OswAppWatchfaceMix());
  watchFaceSwitcher.registerApp(new OswAppWatchfaceBinary());
  mainAppSwitcher.registerApp(&watchFaceSwitcher);

  mainAppSwitcher.setup();

#ifdef RAW_SCREEN_SERVER
  //  screenserver_setup(hal);
  screenserver_setup();
#endif

#if USE_ULP == 1
  // register the ULP program
  init_ulp();
#endif

#ifndef NDEBUG
  Serial.println("Setup Done");
#endif
}

void loop() {
  static time_t lastPowerUpdate = time(nullptr) + 2;  // We consider a run of at least 2 seconds as "success"
  static boolean delayedAppInit = true;

// check possible interaction with ULP program
#if USE_ULP == 1
  loop_ulp();
#endif

  try {
    hal->handleWakeupFromLightSleep();
    hal->checkButtons();
    hal->devices->update();
    // update power statistics only when WiFi isn't used - fixing:
    // https://github.com/Open-Smartwatch/open-smartwatch-os/issues/163
    bool wifiDisabled = true;
    #ifdef OSW_FEATURE_WIFI
      wifiDisabled = !OswServiceAllTasks::wifi.isEnabled();
    #endif
    if (time(nullptr) != lastPowerUpdate && wifiDisabled) {
      // Only update those every second
      hal->updatePowerStatistics(hal->getBatteryRaw(20));
      lastPowerUpdate = time(nullptr);
    }
  } catch(const std::runtime_error& e) {
    Serial.println(String("CRITICAL ERROR AT UPDATES: ") + e.what());
    sleep(10);
    ESP.restart();
  }

  // Now update the screen (this will maybe sleep for a while)
  try {
    OswUI::getInstance()->loop(mainAppSwitcher, mainAppIndex);
  } catch(const std::runtime_error& e) {
    Serial.println(String("CRITICAL ERROR AT APP: ") + e.what());
    sleep(10);
    ESP.restart();
  }
  if (delayedAppInit) {
    // fix flickering display on latest Arduino_GFX library
    ledcWrite(1, OswConfigAllKeys::settingDisplayBrightness.get());
  }

#ifdef RAW_SCREEN_SERVER
  //  screenserver_loop(hal);
  screenserver_loop();
#endif

  if (delayedAppInit) {
    delayedAppInit = false;
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
    mainAppSwitcher.registerApp(new OswAppMap());
#endif
#if OSW_PLATFORM_ENVIRONMENT_MAGNETOMETER == 1 && OSW_PLATFORM_HARDWARE_QMC5883L == 1
    mainAppSwitcher.registerApp(new OswAppMagnetometerCalibrate());
#endif
    // For a short howto write your own apps see: app below
    // mainAppSwitcher.registerApp(new OswAppHelloWorld());

    // tools
#if TOOL_STOPWATCH == 1
    mainAppSwitcher.registerApp(new OswAppStopWatch());
#endif
#if TOOL_WATERLEVEL == 1
    mainAppSwitcher.registerApp(new OswAppWaterLevel());
#endif

    // config
#ifdef OSW_FEATURE_WIFI
    settingsAppSwitcher.registerApp(new OswAppWebserver());
#endif

    settingsAppSwitcher.registerApp(new OswAppTimeConfig(&settingsAppSwitcher));
#ifndef NDEBUG
    settingsAppSwitcher.registerApp(new OswAppPrintDebug());
#endif
    settingsAppSwitcher.paginationEnable();
    mainAppSwitcher.registerApp(&settingsAppSwitcher);

    // games
#if GAME_SNAKE == 1
    mainAppSwitcher.registerApp(new OswAppSnakeGame());
#endif

#if GAME_BRICK_BREAKER == 1
    mainAppSwitcher.registerApp(new OswAppBrickBreaker());
#endif

#ifdef OSW_FEATURE_LUA
    mainAppSwitcher.registerApp(new OswLuaApp("stopwatch.lua"));
#endif
  }

#ifndef NDEBUG
  OswServiceAllTasks::memory.updateLoopTaskStats();
#endif
}
