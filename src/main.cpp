#include <Arduino.h>
#include <Wire.h>
#include <config.h>
#include <osw_hal.h>
#include <osw_pins.h>
#include <launcher/osw_plauncher.h>

#ifndef WIFI_SSID
#pragma error "!!!!!!!!"
#pragma error "PLEASE COPY include/config.h.example TO include/config.h"
#pragma error "AND CONFIGURE THE DEFINES FOR YOUR WATCH"
#pragma error "!!!!!!!!"
#endif

#if defined(GPS_EDITION)
#include "./apps/main/map.h"
#endif
#if defined(BLUETOOTH_COMPANION)
#include "./services/companionservice.h"
#endif
#include "./services/servicemanager.h"
#include "services/services.h"

OswHal *hal = new OswHal();
OswLauncher* launcher = new OswPLauncher();
// OswAppRuntimeTest *runtimeTest = new OswAppRuntimeTest();


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
  launcher->setup(hal); // Will handle all app starts
}

void loop() {

  hal->checkButtons();
  hal->updateAccelerometer();
  launcher->loop();
  // runtimeTest->loop(hal);
}
