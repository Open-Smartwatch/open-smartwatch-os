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

#if SERVICE_WIFI == 1
#ifndef CONFIG_WIFI_SSID
#pragma error "!!!!!!!!"
#pragma error "PLEASE COPY include/config.h.example TO include/config.h"
#pragma error "AND CONFIGURE THE DEFINES FOR YOUR WATCH"
#pragma error "!!!!!!!!"
#endif
#endif

// #include "./apps/_experiments/runtime_test.h"
#include "./apps/_experiments/hello_world.h"
#ifdef LUA_SCRIPTS
#include "./apps/main/luaapp.h"
#endif
#include "./apps/games/snake_game.h"
#if SERVICE_WIFI == 1
#include "./apps/main/OswAppWebserver.h"
#endif
#include "./apps/main/stopwatch.h"
#include "./apps/main/switcher.h"
#include "./apps/tools/button_test.h"
#include "./apps/tools/print_debug.h"
#include "./apps/tools/time_config.h"
#include "./apps/tools/water_level.h"
#include "./apps/watchfaces/watchfacemax.h"
#include "./apps/watchfaces/watchface.h"
#include "./apps/watchfaces/watchface_binary.h"
#include "./apps/watchfaces/watchface_digital.h"
#include "./overlays/overlays.h"
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
#include "./apps/_experiments/compass_calibrate.h"
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

  //Begin the i2c interface make sure to never init it again.
  Wire.begin(SDA, SCL, 100000L); 

  // Load config as early as possible, to ensure everyone can access it.
  OswConfig::getInstance()->setup();

  // First setup the bare minimum to get a watchface on the screen
  hal->setupPower();
  hal->setupTime();      
  hal->setupDisplay();   
  watchFaceSwitcher->registerApp(new OswAppWatchfaceMax());
  mainAppSwitcher->registerApp(watchFaceSwitcher);
  mainAppSwitcher->setup(hal);
  ledcWrite(1, OswConfigAllKeys::settingDisplayBrightness.get());  // fix flickering display on latest Arduino_GFX library
  if(hal->isRequestFlush()){hal->flushCanvas();}

  //setup the rest of the system internals:
  hal->setupButtons();
  hal->setupSensors();
  hal->setupFileSystem();

  OswUI::getInstance()->setup(hal);
  OswServiceManager::getInstance().setup(hal);

//Register all the other apps except the first watch face
  // enable your watchfaces here:
  watchFaceSwitcher->registerApp(new OswAppWatchface());
  watchFaceSwitcher->registerApp(new OswAppWatchfaceDigital());
  watchFaceSwitcher->registerApp(new OswAppWatchfaceBinary());
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
#ifdef LUA_SCRIPTS
  mainAppSwitcher->registerApp(new OswLuaApp("stopwatch.lua"));
#endif
  // config
  settingsAppSwitcher->registerApp(new OswAppWebserver());
  settingsAppSwitcher->registerApp(new OswAppTimeConfig());
  mainAppSwitcher->registerApp(settingsAppSwitcher);


  // Make sure the RTC is loaded and get the real time (!= 0, other than time(nullptr), which is possibly 0 right now)
  randomSeed(hal->getUTCTime()); 

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
#if USE_ULP == 1
  // check possible interaction with ULP program
  loop_ulp();
#endif

  mainAppSwitcher->loop(hal);
  hal->handleWakeupFromLightSleep();
  hal->checkButtons();
  hal->fetchRtcTime();         //TODO - doesn't need every cycle update. 
  hal->updateAccelerometer();

  // limit to 30 fps and handle display flushing
  static long lastFlush = 0; 
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

#ifdef DEBUG
  OswServiceAllTasks::memory.updateLoopTaskStats();
#endif
}
