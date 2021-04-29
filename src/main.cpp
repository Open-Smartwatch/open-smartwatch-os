#include <Arduino.h>
#include <Wire.h>
#include <config.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_pins.h>

#ifndef WIFI_SSID
#pragma error "!!!!!!!!"
#pragma error "PLEASE COPY include/config.h.example TO include/config.h"
#pragma error "AND CONFIGURE THE DEFINES FOR YOUR WATCH"
#pragma error "!!!!!!!!"
#endif

// #include "./apps/_experiments/runtime_test.h"
#include "./apps/_experiments/hello_world.h"
#include "./apps/main/luaapp.h"
#include "./apps/main/stopwatch.h"
#include "./apps/main/watchface.h"
#include "./apps/tools/print_debug.h"
#include "./apps/tools/time_from_web.h"
#include "./apps/tools/water_level.h"
#include "./overlays/overlays.h"
#include "apps/lua/mylua_example.h"
#if defined(GPS_EDITION)
#include "./apps/main/map.h"
#endif
#if defined(BLUETOOTH_COMPANION)
#include "./services/companionservice.h"
#endif
#include "./services/servicemanager.h"
#include "services/services.h"

OswHal *hal = new OswHal();
// OswAppRuntimeTest *runtimeTest = new OswAppRuntimeTest();

// HINT: NUM_APPS must match the number of apps below!
#if defined(GPS_EDITION)
#define NUM_APPS 5
#else
#define NUM_APPS 4
#endif
RTC_DATA_ATTR uint8_t appPtr = 0;
OswApp *mainApps[] = {
    new OswAppWatchface(),
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

void setup() {
  Serial.begin(115200);

  hal->setupPower();
  hal->setupButtons();
  hal->setupSensors();

  hal->setupDisplay();
  hal->setBrightness(128);

  xTaskCreatePinnedToCore(core2Worker, "core2Worker", 1000 /*stack*/, NULL /*input*/, 0 /*prio*/,
                          &Core2WorkerTask /*handle*/, 0);

  OswServiceManager &serviceManager = OswServiceManager::getInstance();
  serviceManager.setup(hal);  // Services should always start before apps do
  mainApps[appPtr]->setup(hal);
}

void loop() {
  static long lastFlush = 0;
  static unsigned long appOnScreenSince = millis();
  static long lastBtn1Duration = 0;

  hal->checkButtons();
  hal->updateAccelerometer();

  if (hal->btn1Down()) {
    lastBtn1Duration = hal->btn1Down();
  }

  // handle long click to sleep
  if (!hal->btn1Down() && lastBtn1Duration >= BTN_1_SLEEP_TIMEOUT) {
    hal->getCanvas()->getGraphics2D()->fill(rgb565(0, 0, 0));
    hal->flushCanvas();
    hal->deepSleep();
  }

  // handle medium click to switch
  if (!hal->btn1Down() && lastBtn1Duration >= BTN_1_APP_SWITCH_TIMEOUT) {
    // switch app
    mainApps[appPtr]->stop(hal);
    appPtr++;
    appPtr %= NUM_APPS;
    mainApps[appPtr]->setup(hal);
    appOnScreenSince = millis();

    lastBtn1Duration = 0;
  }

  mainApps[appPtr]->loop(hal);
  // runtimeTest->loop(hal);

  // limit to 30 fps and handle display flushing
  if (millis() - lastFlush > 1000 / 30 && hal->isRequestFlush()) {
    drawOverlays(hal);
    hal->flushCanvas();
    lastFlush = millis();
  }

  // auto sleep on first screen
  if (appPtr == 0 && (millis() - appOnScreenSince) > 5000) {
    hal->gfx()->fill(rgb565(0, 0, 0));
    hal->flushCanvas();
    hal->deepSleep();
  }
}
