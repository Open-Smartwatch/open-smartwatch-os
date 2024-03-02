#include <Arduino.h>
#include <Wire.h>
#include <config_defaults.h>
#include <OswAppV1.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <osw_pins.h>
#include <osw_ui.h>
#include <osw_ulp.h>
#include <OswLogger.h>
#include <time.h>    //time

#ifdef OSW_FEATURE_WIFI
#ifndef CONFIG_WIFI_SSID
#pragma error "!!!!!!!!"
#pragma error "PLEASE COPY include/config.h.example TO include/config.h"
#pragma error "AND CONFIGURE THE DEFINES FOR YOUR WATCH"
#pragma error "!!!!!!!!"
#endif
#endif

// #include "./apps/_experiments/runtime_test.h"
#include "apps/OswAppV2Compat.h"
#include "apps/OswAppDrawer.h"
#include "apps/examples/OswAppExampleV1.h"
#include "apps/examples/OswAppExampleV2.h"
#include "apps/tools/OswAppTutorial.h"
#ifdef OSW_FEATURE_LUA
#include "./apps/main/luaapp.h"
#endif
#include "assets/img/icons/brickbreaker.png.h"
#include "./apps/games/brick_breaker.h"
#include "assets/img/icons/snake.png.h"
#include "./apps/games/snake_game.h"
#ifdef OSW_FEATURE_WIFI
#include "assets/img/icons/settings.png.h"
#include "./apps/tools/OswAppWebserver.h"
#endif
#include "assets/img/icons/stopwatch.png.h"
#include "./apps/clock/stopwatch.h"
#include "assets/img/icons/alarm.png.h"
#include "./apps/clock/OswAppAlarm.h"
#include "assets/img/icons/timer.png.h"
#include "./apps/clock/OswAppTimer.h"
#include "assets/img/icons/calculator.png.h"
#include "./apps/tools/OswAppCalculator.h"
#include "./apps/tools/OswAppFlashLight.h"
#include "./apps/tools/button_test.h"
#ifndef NDEBUG
#include "./apps/tools/OswAppPrintDebug.h"
#endif
#include "assets/img/icons/time.png.h"
#include "./apps/tools/OswAppTimeConfig.h"
#include "assets/img/icons/waterlevel.png.h"
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
#include "./apps/watchfaces/OswAppWatchfaceFitnessAnalog.h"
#if OSW_PLATFORM_ENVIRONMENT_MAGNETOMETER == 1 && OSW_PLATFORM_HARDWARE_QMC5883L == 1
#include "./apps/_experiments/magnetometer_calibrate.h"
#endif
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
#include "./apps/main/map.h"
#endif
#include "./services/OswServiceTaskBLECompanion.h"
#include "services/OswServiceTaskMemMonitor.h"
#include "services/OswServiceTasks.h"
#ifdef OSW_FEATURE_WIFI
#include <services/OswServiceTaskWiFi.h>
#endif
#ifdef OSW_FEATURE_WEATHER
#include "./apps/_experiments/OswAppWeather.h"
#endif

// get global variables (make sure to NOT include any headers after the "using" statements!)
#include "globals.h"
using OswGlobals::main_mainDrawer;
using OswGlobals::main_tutorialApp;

#ifndef NDEBUG
#define _MAIN_CRASH_SLEEP 10
#else
#define _MAIN_CRASH_SLEEP 2
#endif

void setup() {
    Serial.begin(115200);
    OSW_LOG_I("Welcome to the OSW-OS! This build is based on commit ", GIT_COMMIT_HASH, " from ", GIT_BRANCH_NAME,
              ". Compiled at ", __DATE__, " ", __TIME__, " for platform ", PIO_ENV_NAME, ".");

    // Load config as early as possible, to ensure everyone can access it.
    OswConfig::getInstance()->setup();

    // First setup hardware/sensors/display -> might be used by background services
    try {
        OswHal::getInstance()->setup(false);
    } catch(const std::exception& e) {
        OSW_LOG_E("CRITICAL ERROR AT BOOTUP: ", + e.what());
        sleep(_MAIN_CRASH_SLEEP);
        ESP.restart();
    }

    // TODO port all v1 watchfaces to v2, to allow for lazy loading
    main_mainDrawer.registerAppLazy<OswAppWatchface>(LANG_WATCHFACES);
    main_mainDrawer.registerAppLazy<OswAppWatchfaceDigital>(LANG_WATCHFACES);
    main_mainDrawer.registerAppLazy<OswAppWatchfaceMix>(LANG_WATCHFACES);
    main_mainDrawer.registerAppLazy<OswAppWatchfaceDual>(LANG_WATCHFACES);
    main_mainDrawer.registerAppLazy<OswAppWatchfaceFitness>(LANG_WATCHFACES);
    main_mainDrawer.registerAppLazy<OswAppWatchfaceBinary>(LANG_WATCHFACES);
    main_mainDrawer.registerAppLazy<OswAppWatchfaceMonotimer>(LANG_WATCHFACES);
    main_mainDrawer.registerAppLazy<OswAppWatchfaceNumerals>(LANG_WATCHFACES);
    main_mainDrawer.registerAppLazy<OswAppWatchfaceFitnessAnalog>(LANG_WATCHFACES);
    try {
        main_mainDrawer.startApp(OswConfigAllKeys::settingDisplayDefaultWatchface.get().c_str()); // if this id is invalid, the drawer will fall back to alternatives automatically
    } catch(const std::runtime_error& e) {
        OSW_LOG_E("Could not start default watchface: ", e.what());
    }

    // Install drawer and (maybe) jump into tutorial
    OswUI::getInstance()->setRootApplication(&main_mainDrawer);
    main_tutorialApp.reset(new OswAppTutorial());
    if(!main_tutorialApp->changeRootAppIfNecessary())
        main_tutorialApp.reset(); // no need to keep it around, as it's not the root app

#if USE_ULP == 1
    // register the ULP program
    init_ulp();
#endif

    OSW_LOG_D("Setup Done");
}

void loop() {
    static time_t lastPowerUpdate = time(nullptr) + 2;  // We consider a run of at least 2 seconds as "success"
    static time_t nextTimezoneUpdate = time(nullptr) + 60; // Already done after sleep -> revisit in a while
    static bool delayedAppInit = true;

// check possible interaction with ULP program
#if USE_ULP == 1
    loop_ulp();
#endif

    try {
        OswHal::getInstance()->handleDisplayTimout();
        OswHal::getInstance()->handleWakeupFromLightSleep();
        OswHal::getInstance()->checkButtons();
        OswHal::getInstance()->devices()->update();
        // update power statistics only when WiFi isn't used - fixing:
        // https://github.com/Open-Smartwatch/open-smartwatch-os/issues/163
        bool wifiDisabled = true;
#ifdef OSW_FEATURE_WIFI
        wifiDisabled = !OswServiceAllTasks::wifi.isEnabled();
#endif
        if (time(nullptr) > lastPowerUpdate and wifiDisabled) {
            // Only update those every second
            OswHal::getInstance()->updatePowerStatistics(OswHal::getInstance()->getBatteryRaw(20));
            lastPowerUpdate = time(nullptr);
        }
        if(time(nullptr) > nextTimezoneUpdate) {
            OswHal::getInstance()->updateTimezoneOffsets();
            nextTimezoneUpdate = time(nullptr) + 60; // Update every minute
        }
    } catch(const std::exception& e) {
        OSW_LOG_E("CRITICAL ERROR AT UPDATES: ", e.what());
        sleep(_MAIN_CRASH_SLEEP);
        ESP.restart();
    }

    // Now update the screen (this will maybe sleep for a while)
    try {
        OswUI::getInstance()->loop();
    } catch(const std::exception& e) {
        OSW_LOG_E("CRITICAL ERROR AT APP: ", e.what());
        sleep(_MAIN_CRASH_SLEEP);
        ESP.restart();
    }
    if (delayedAppInit) {
        // fix flickering display on latest Arduino_GFX library
        ledcWrite(1, OswConfigAllKeys::settingDisplayBrightness.get());
    }

    if (delayedAppInit) {
        delayedAppInit = false;

        // TODO port all v1 apps to v2, to allow for lazy loading (or let them in compat mode)

        // GPS
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
        static OswAppMap gpsOswAppMap;
        main_mainDrawer.registerApp("GPS", new OswAppV2Compat("osw.gps.map", "Map", gpsOswAppMap));
#endif
#if OSW_PLATFORM_ENVIRONMENT_MAGNETOMETER == 1 && OSW_PLATFORM_HARDWARE_QMC5883L == 1
        static OswAppMagnetometerCalibrate gpsOswAppMC;
        main_mainDrawer.registerApp("GPS", new OswAppV2Compat("osw.gps.mc", "Magnetometer Calibrate", gpsOswAppMC));
#endif

#if OSW_APPS_EXAMPLES == 1
        // For a short "How to write your own apps" see the examples below
        main_mainDrawer.registerAppLazy<OswAppExampleV2>(LANG_EXAMPLES); // only v2 apps support lazy loading (for now)
        static OswAppExampleV1 exampleV1; // this is a static object, so it will be kept in memory until the device is reset - but it kind of defeats the purpose of lazy loading (static object = initialized on bootup, not on first use)
        main_mainDrawer.registerApp(LANG_EXAMPLES, new OswAppV2Compat("osw.example.v1", LANG_EXAMPLE " " LANG_APP " v1", exampleV1));
        main_mainDrawer.registerAppLazy<OswAppTutorial>(LANG_EXAMPLES); // also add the tutorial to the examples list
#endif

        // Fitness App
#ifdef OSW_FEATURE_STATS_STEPS
        static OswAppStepStats fitnessStepStats;
        static OswAppKcalStats fitnessKcalStats;
        static OswAppDistStats fitnessDistStats;
        main_mainDrawer.registerApp(LANG_FITNESS, new OswAppV2Compat("osw.fit.ss", "Step Statistics", fitnessStepStats, false));
        main_mainDrawer.registerApp(LANG_FITNESS, new OswAppV2Compat("osw.fit.ks", "Kcal Statistics", fitnessKcalStats, false));
        main_mainDrawer.registerApp(LANG_FITNESS, new OswAppV2Compat("osw.fit.ds", "Distance Statistics", fitnessDistStats, false));
#endif
        static OswAppFitnessStats fitnessStats;
        main_mainDrawer.registerApp(LANG_FITNESS, new OswAppV2Compat("osw.fit.fs", "Fitness Statistics", fitnessStats));

        // Tools
#if TOOL_CLOCK == 1
        static OswAppStopWatch toolStopWatch;
        static OswAppTimer toolTimer;
        static OswAppAlarm toolAlarm;
        main_mainDrawer.registerApp(LANG_TOOLS, new OswAppV2Compat("osw.tool.sw", "Stopwatch", toolStopWatch, true, stopwatch_png));
        main_mainDrawer.registerApp(LANG_TOOLS, new OswAppV2Compat("osw.tool.tm", "Timer", toolTimer, true, timer_png));
        main_mainDrawer.registerApp(LANG_TOOLS, new OswAppV2Compat("osw.tool.al", "Alarm", toolAlarm, true, alarm_png));
#endif
#if TOOL_FLASHLIGHT == 1
        main_mainDrawer.registerAppLazy<OswAppFlashLight>(LANG_TOOLS);
#endif
#if TOOL_WATERLEVEL == 1
        static OswAppWaterLevel toolWaterLevel;
        main_mainDrawer.registerApp(LANG_TOOLS, new OswAppV2Compat("osw.tool.wl", "Water Level", toolWaterLevel, true, waterlevel_png));
#endif
#if TOOL_CALCULATOR == 1
        static OswAppCalculator toolCalculator;
        main_mainDrawer.registerApp(LANG_TOOLS, new OswAppV2Compat("osw.tool.ca", "Calculator", toolCalculator, true, calculator_png));
#endif

        // Weather
#ifdef OSW_FEATURE_WEATHER
        static OswAppWeather weather;
        main_mainDrawer.registerApp(LANG_WEATHER, new OswAppV2Compat("osw.weather", "Weather", weather));
#endif

        // Settings
#ifdef OSW_FEATURE_WIFI
        static OswAppWebserver configWifi;
        main_mainDrawer.registerApp(LANG_SETTINGS, new OswAppV2Compat("osw.sets.wifi", "WiFi", configWifi, true, settings_png));
#endif

        static OswAppTimeConfig configTime;
        main_mainDrawer.registerApp(LANG_SETTINGS, new OswAppV2Compat("osw.sets.time", "Time", configTime, true, time_png));
#ifndef NDEBUG
        static OswAppPrintDebug configDebug;
        main_mainDrawer.registerApp(LANG_SETTINGS, new OswAppV2Compat("osw.sets.debug", "Debug", configDebug));
#endif

        // Games
#if GAME_SNAKE == 1
        static OswAppSnakeGame gameSnake;
        main_mainDrawer.registerApp(LANG_GAMES, new OswAppV2Compat("osw.game.snake", "Snake", gameSnake, true, snake_png));
#endif
#if GAME_BRICK_BREAKER == 1
        static OswAppBrickBreaker gameBrickBreaker;
        main_mainDrawer.registerApp(LANG_GAMES, new OswAppV2Compat("osw.game.brick", "Brick Breaker", gameBrickBreaker, true, brickbreaker_png));
#endif

#ifdef OSW_FEATURE_LUA
        static OswLuaApp luaApp("stopwatch.lua");
        main_mainDrawer.registerApp("LUA", new OswAppV2Compat("osw.lua", "Demo", luaApp));
#endif
    }

#ifndef OSW_EMULATOR
    OswServiceAllTasks::memory.updateLoopTaskStats();
#endif
}
