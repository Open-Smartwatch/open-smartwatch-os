#include <Arduino.h>
#include <Wire.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_pins.h>

#include "./apps/main/watchface.h"
#include "./apps/tools/ble_media_ctrl.h"
#include "./apps/tools/print_debug.h"
#include "./apps/tools/water_level.h"

OswHal *hal = new OswHal();
OswAppBLEMEdiaCtrl *bleCtrl = new OswAppBLEMEdiaCtrl();

#if defined(GPS_EDITION)
#include "esp_task_wdt.h"
TaskHandle_t Task1;
void backgroundLoop(void *pvParameters) {
  while (true) {
    hal->gpsParse();
    delay(1);
  }
}
#endif

void IRAM_ATTR isrStepDetect() { Serial.println("Step"); }

void setup() {
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, LOW);
  Serial.begin(115200);

  hal->setupPower();
  hal->setupButtons();
  hal->setupSensors();
  hal->setupDisplay();
  hal->setBrightness(128);

#if defined(GPS_EDITION)
  hal->setupGps();
  hal->setupSD();

  xTaskCreatePinnedToCore(backgroundLoop, "backgroundLoop", 1000 /*stack*/, NULL /*input*/, 0 /*prio*/,
                          &Task1 /*handle*/, 0);
#endif

  bleCtrl->setup(hal);
}

bool printDebugInfo = false;
void loop() {
  hal->checkButtons();
  hal->updateAccelerometer();

  // call your watch app loop here

  // fadeIn->loop(hal);
  // autumn->loop(hal);
  // displaySize->loop(hal);
  // powerDemo->loop(hal);
  // waterLevel->loop(hal);
  // appFireworks->loop(hal);
  // paulsWatchFace->loop(hal);
  // runtimeTest->loop(hal);
  bleCtrl->loop(hal);

  // if (hal->btn1Down()) {
  //   printDebugInfo = !printDebugInfo;
  //   hal->getCanvas()->getGraphics2D()->fill(rgb565(0,100,0));
  //   hal->flushCanvas();
  //   hal->setDebugGPS(printDebugInfo);
  //   delay(250);
  // }
  // if (printDebugInfo) {
  // printDebug->loop(hal);
  // } else {
  //   osmMap->loop(hal);
  // }

  // Serial.println(hal->getAccelerationX());

  // hal->deepSleep();
  delay(1000 / 30);
}
