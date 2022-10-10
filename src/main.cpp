#include <Arduino.h>
#include <Wire.h>
#include <config_defaults.h>
#include <osw_app.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <osw_pins.h>
#include <osw_ui.h>
#include <osw_ulp.h>
#include <stdlib.h>  //randomSeed
#include <time.h>    //time

#ifdef OSW_FEATURE_WIFI
#ifndef CONFIG_WIFI_SSID
#pragma error "!!!!!!!!"
#pragma error "PLEASE COPY include/config.h.example TO include/config.h"
#pragma error "AND CONFIGURE THE DEFINES FOR YOUR WATCH"
#pragma error "!!!!!!!!"
#endif
#endif
#ifdef OSW_FEATURE_WIFI
#ifdef OSW_FEATURE_BLE
#error "The RAM on all current OSW models is not big enough to hold both WiFi AND Bluetooth stacks during runtime. This WILL lead to a crash. Please only use one of these features simultaneously!"
#endif
#endif
#ifdef RAW_SCREEN_SERVER
#ifndef OSW_FEATURE_WIFI
#error "OSW_FEATURE_WIFI is not declared. please declare and re-build."
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
#include "./apps/tools/OswAppWebserver.h"
#endif
#include "./apps/main/stopwatch.h"
#include "./apps/main/switcher.h"
#include "./apps/tools/button_test.h"
#ifndef NDEBUG
#include "./apps/tools/OswAppPrintDebug.h"
#endif
#include "./apps/tools/OswAppTimeConfig.h"
#include "./apps/tools/OswAppWaterLevel.h"
#include "./apps/tools/OswAppFitnessStats.h"
#ifdef OSW_FEATURE_STATS_STEPS
#include "./apps/tools/OswAppKcalStats.h"
#include "./apps/tools/OswAppStepStats.h"
#include "./apps/tools/OswAppDistStats.h"
#endif
#include "./apps/watchfaces/OswAppWatchface.h"
#include "./apps/watchfaces/OswAppWatchfaceDigital.h"
#include "./apps/watchfaces/OswAppWatchfaceMix.h"
#include "./apps/watchfaces/OswAppWatchfaceDual.h"
#include "./apps/watchfaces/OswAppWatchfaceFitness.h"
#include "./apps/watchfaces/OswAppWatchfaceBinary.h"
#include "./apps/watchfaces/OswAppWatchfaceMonotimer.h"
#include "./apps/watchfaces/OswAppWatchfaceNumerals.h"
#if OSW_PLATFORM_ENVIRONMENT_MAGNETOMETER == 1 && OSW_PLATFORM_HARDWARE_QMC5883L == 1
#include "./apps/_experiments/magnetometer_calibrate.h"
#endif
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
#include "./apps/main/map.h"
#endif
#ifdef OSW_FEATURE_BLE
#include "./services/OswServiceTaskBLECompanion.h"
#endif
#include "services/OswServiceTaskMemMonitor.h"
#include "services/OswServiceTasks.h"
#ifdef OSW_FEATURE_WIFI
#include <services/OswServiceTaskWiFi.h>
#endif
#include "globals.h"
#ifdef OSW_FEATURE_BLE_MEDIA_CTRL
#include <apps/tools/OswAppBLEMEdiaCtrl.h>
#endif

#ifndef NDEBUG
#define _MAIN_CRASH_SLEEP 10
#else
#define _MAIN_CRASH_SLEEP 2
#endif

OswAppSwitcher mainAppSwitcher(BUTTON_1, LONG_PRESS, true, true, &main_currentAppIndex);
OswAppSwitcher watchFaceSwitcher(BUTTON_1, SHORT_PRESS, false, false, &main_watchFaceIndex);
OswAppSwitcher settingsAppSwitcher(BUTTON_1, SHORT_PRESS, false, false, &main_settingsAppIndex);
OswAppSwitcher fitnessAppSwitcher(BUTTON_1, SHORT_PRESS, false, false, &main_fitnessAppIndex);

void setup() {
    Serial.begin(115200);
    Serial.println(String("Welcome to the OSW-OS! This build is based on commit ") + GIT_COMMIT_HASH + " from " + GIT_BRANCH_NAME +
                   ". Compiled at " + __DATE__ + " " + __TIME__ + " for platform " + PIO_ENV_NAME + ".");

    // Load config as early as possible, to ensure everyone can access it.
    OswConfig::getInstance()->setup();
    main_watchFaceIndex = OswConfigAllKeys::settingDisplayDefaultWatchface.get().toInt();

    // First setup hardware/sensors/display -> might be used by background services
    try {
        OswHal::getInstance()->setup(false);
    } catch(const std::runtime_error& e) {
        Serial.println(String("CRITICAL ERROR AT BOOTUP: ") + e.what());
        sleep(_MAIN_CRASH_SLEEP);
        ESP.restart();
    }

    watchFaceSwitcher.registerApp(new OswAppWatchface());
    watchFaceSwitcher.registerApp(new OswAppWatchfaceDigital());
    watchFaceSwitcher.registerApp(new OswAppWatchfaceMix());
    watchFaceSwitcher.registerApp(new OswAppWatchfaceDual());
    watchFaceSwitcher.registerApp(new OswAppWatchfaceFitness());
    watchFaceSwitcher.registerApp(new OswAppWatchfaceBinary());
    watchFaceSwitcher.registerApp(new OswAppWatchfaceMonotimer());
    watchFaceSwitcher.registerApp(new OswAppWatchfaceNumerals());
    mainAppSwitcher.registerApp(&watchFaceSwitcher);

    mainAppSwitcher.setup();

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
    static bool delayedAppInit = true;

// check possible interaction with ULP program
#if USE_ULP == 1
    loop_ulp();
#endif

    try {
        OswHal::getInstance()->handleWakeupFromLightSleep();
        OswHal::getInstance()->checkButtons();
        OswHal::getInstance()->devices->update();
        // update power statistics only when WiFi isn't used - fixing:
        // https://github.com/Open-Smartwatch/open-smartwatch-os/issues/163
        bool wifiDisabled = true;
#ifdef OSW_FEATURE_WIFI
        wifiDisabled = !OswServiceAllTasks::wifi.isEnabled();
#endif
        if (time(nullptr) != lastPowerUpdate && wifiDisabled) {
            // Only update those every second
            OswHal::getInstance()->updatePowerStatistics(OswHal::getInstance()->getBatteryRaw(20));
            lastPowerUpdate = time(nullptr);
        }
    } catch(const std::runtime_error& e) {
        Serial.println(String("CRITICAL ERROR AT UPDATES: ") + e.what());
        sleep(_MAIN_CRASH_SLEEP);
        ESP.restart();
    }

    // Now update the screen (this will maybe sleep for a while)
    try {
        OswUI::getInstance()->loop(mainAppSwitcher, main_currentAppIndex);
    } catch(const std::runtime_error& e) {
        Serial.println(String("CRITICAL ERROR AT APP: ") + e.what());
        sleep(_MAIN_CRASH_SLEEP);
        ESP.restart();
    }
    if (delayedAppInit) {
        // fix flickering display on latest Arduino_GFX library
        ledcWrite(1, OswConfigAllKeys::settingDisplayBrightness.get());
    }

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

        // Fitness App
#ifdef OSW_FEATURE_STATS_STEPS
        fitnessAppSwitcher.registerApp(new OswAppStepStats());
        fitnessAppSwitcher.registerApp(new OswAppKcalStats());
        fitnessAppSwitcher.registerApp(new OswAppDistStats());
#endif
        fitnessAppSwitcher.registerApp(new OswAppFitnessStats());
        fitnessAppSwitcher.paginationEnable();
        mainAppSwitcher.registerApp(&fitnessAppSwitcher);
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
#ifdef OSW_FEATURE_BLE_MEDIA_CTRL
        settingsAppSwitcher.registerApp(new OswAppBLEMEdiaCtrl());
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

#ifndef OSW_EMULATOR
    OswServiceAllTasks::memory.updateLoopTaskStats();
#endif
}
