#include <Arduino.h>
#include <Wire.h>
#include <osw_app.h>
#include <osw_app_autumn.h>
#include <osw_app_fadein_display.h>
#include <osw_app_fireworks.h>
#include <osw_app_map.h>
#include <osw_app_power_demo.h>
#include <osw_app_print_debug.h>
#include <osw_app_show_display_size.h>
#include <osw_app_watchface_demo.h>
#include <osw_app_water_level.h>
#include <osw_hal.h>

#include "WiFi.h"

OswHal *hal = new OswHal();
OswAppPrintDebug *printDebug = new OswAppPrintDebug();
OswAppFadeInDisplay *fadeIn = new OswAppFadeInDisplay(8 * 255);
// OswAppWatchFaceDemo *watchFace = new OswAppWatchFaceDemo();
OswAppAutumn *autumn = new OswAppAutumn();
OswAppShowDisplaySize *displaySize = new OswAppShowDisplaySize();
OswAppPowerDemo *powerDemo = new OswAppPowerDemo();
OswAppWaterLevel *waterLevel = new OswAppWaterLevel();
OswAppMap *osmMap = new OswAppMap();
OswAppFireworks *appFireworks = new OswAppFireworks();

#include "esp_task_wdt.h"
TaskHandle_t Task1;
void backgroundLoop(void *pvParameters) {
  while (true) {
    hal->gpsParse();
    delay(1);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  digitalWrite(2, 1);

  hal->setupPower();
  hal->setupButtons();
  hal->setupSensors();
  hal->setupDisplay();
  hal->setupGps();
  hal->setupSD();

  hal->setBrightness(200);

  xTaskCreatePinnedToCore(backgroundLoop, "backgroundLoop", 1000 /*stack*/, NULL /*input*/, 0 /*prio*/,
                          &Task1 /*handle*/, 0);

  // call your watch app setup here
  // autumn->setup(hal);
  // powerDemo->setup(hal);
  // osmMap->setup(hal);
  appFireworks->setup(hal);
}

bool printDebugInfo = false;
void loop() {
  hal->checkButtons();
  hal->updateAccelerometer();

  // call your watch app loop here

  fadeIn->loop(hal);
  // autumn->loop(hal);
  // displaySize->loop(hal);
  // powerDemo->loop(hal);
  // waterLevel->loop(hal);
  appFireworks->loop(hal);

  // if (hal->btn1Down()) {
  //   printDebugInfo = !printDebugInfo;
  //   hal->getCanvas()->getGraphics2D()->fill(rgb565(0,100,0));
  //   hal->flushCanvas();
  //   hal->setDebugGPS(printDebugInfo);
  //   delay(250);
  // }
  // if (printDebugInfo) {
  //   printDebug->loop(hal);
  // } else {
  //   osmMap->loop(hal);
  // }

  hal->flushCanvas();
  // hal->deepSleep();
}
