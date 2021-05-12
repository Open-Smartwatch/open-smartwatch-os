#include <Arduino.h>
#include <Wire.h>
#include <config.h>
#include <osw_app.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <osw_pins.h>
#include <stdlib.h>  //randomSeed
#include <time.h>    //time

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
#include "./apps/main/stopwatch.h"
#include "./apps/main/switcher.h"
#include "./apps/main/watchface.h"
#include "./apps/main/watchface_binary.h"
#include "./apps/main/watchface_digital.h"
#include "./apps/tools/button_test.h"
#include "./apps/tools/config_mgmt.h"
#include "./apps/tools/print_debug.h"
#include "./apps/tools/time_from_web.h"
#include "./apps/tools/water_level.h"
#include "./overlays/overlays.h"
#if defined(GPS_EDITION)
#include "./apps/main/map.h"
#endif
#if defined(BLUETOOTH_COMPANION)
#include "./services/companionservice.h"
#endif
#include "./services/servicemanager.h"
#include "hal/esp32/spiffs_filesystem.h"
#include "services/services.h"

OswHal *hal = new OswHal(new SPIFFSFileSystemHal());
// OswAppRuntimeTest *runtimeTest = new OswAppRuntimeTest();

// HINT: NUM_APPS must match the number of apps below!
#if defined(GPS_EDITION)
#define NUM_APPS 7
#else
#define NUM_APPS 6
#endif
RTC_DATA_ATTR uint8_t appPtr = 0;

OswAppSwitcher *mainAppSwitcher = new OswAppSwitcher(BUTTON_1, LONG_PRESS, true, true);
OswAppSwitcher *watchFaceSwitcher = new OswAppSwitcher(BUTTON_1, SHORT_PRESS, false, false);

#include "esp_task_wdt.h"
TaskHandle_t Core2WorkerTask;

void registerSystemServices() {
  // Register system services
  OswServiceManager &serviceManager = OswServiceManager::getInstance();

#if defined(BLUETOOTH_COMPANION)
  serviceManager.registerService(Services::BLUETOOTH_COMPANION_SERVICE, new OswServiceCompanion());
#endif
}

void loop_onCore2() {
#if defined(GPS_EDITION)
  // TODO: move to background service
  hal->gpsParse();
#endif

  OswServiceManager &serviceManager = OswServiceManager::getInstance();
  serviceManager.loop(hal);
  delay(1);
}

void setup_onCore2() {
#if defined(GPS_EDITION)
  hal->setupGps();
  hal->setupSD();

  Serial.print("PSRAM free: ");
  Serial.println(ESP.getMinFreePsram());
  Serial.print("Free Memory: ");
  Serial.println((int)xPortGetFreeHeapSize());

#endif
  // Register system services
  registerSystemServices();
}

void core2Worker(void *pvParameters) {
  setup_onCore2();
  while (true) {
    loop_onCore2();
  }
}

short displayTimeout = 0;
void setup() {
  watchFaceSwitcher->registerApp(new OswAppWatchface());
  watchFaceSwitcher->registerApp(new OswAppWatchfaceDigital());
  watchFaceSwitcher->registerApp(new OswAppWatchfaceBinary());
  mainAppSwitcher->registerApp(watchFaceSwitcher);
#ifdef GPS_EDITION
  mainAppSwitcher->registerApp(new OswAppMap());
#endif
  // mainAppSwitcher->registerApp(new OswAppHelloWorld());
  // mainAppSwitcher->registerApp(new OswAppPrintDebug());
  mainAppSwitcher->registerApp(new OswAppStopWatch());
  mainAppSwitcher->registerApp(new OswAppWaterLevel());
  mainAppSwitcher->registerApp(new OswAppTimeFromWeb());
  mainAppSwitcher->registerApp(new OswAppConfigMgmt());
#ifdef LUA_SCRIPTS
  mainAppSwitcher->registerApp(new OswLuaApp("stopwatch.lua"));
#endif

  Serial.begin(115200);
  srand(time(nullptr));

  // Load config as early as possible, to ensure everyone can access it.
  OswConfig::getInstance()->setup();

  hal->setupPower();
  hal->setupFileSystem();
  hal->setupButtons();
  hal->setupSensors();

  hal->setupDisplay();
  hal->setBrightness(128);

  xTaskCreatePinnedToCore(core2Worker, "core2Worker", 1000 /*stack*/, NULL /*input*/, 0 /*prio*/,
                          &Core2WorkerTask /*handle*/, 0);

  OswServiceManager &serviceManager = OswServiceManager::getInstance();
  serviceManager.setup(hal);  // Services should always start before apps do
  mainAppSwitcher->setup(hal);
  displayTimeout = OswConfigAllKeys::displayTimeout.get();
}

void loop() {
  static long lastFlush = 0;

  hal->checkButtons();
  hal->updateAccelerometer();

  mainAppSwitcher->loop(hal);

  // limit to 30 fps and handle display flushing
  if (millis() - lastFlush > 1000 / 30 && hal->isRequestFlush()) {
    drawOverlays(hal);
    hal->flushCanvas();
    lastFlush = millis();
  }
}
