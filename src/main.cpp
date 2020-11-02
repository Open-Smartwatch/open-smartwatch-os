#include <Arduino.h>
#include <osw_app.h>
#include <osw_app_fadein_display.h>
#include <osw_app_print_debug.h>
#include <osw_app_watchface_demo.h>
#include <osw_hal.h>

OswHal *hal = new OswHal();
OswAppPrintDebug *printDebug = new OswAppPrintDebug();
OswAppFadeInDisplay *fadeIn = new OswAppFadeInDisplay(4 * 255);
OswAppWatchFaceDemo *watchFace = new OswAppWatchFaceDemo();

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

  hal->setupPower();
  hal->setupButtons();
  hal->setupSensors();
  hal->setupDisplay();
  hal->setupGps();
  hal->setupSD();

  hal->setBrightness(255);
  hal->gpsAdvancedPowerSave();

  xTaskCreatePinnedToCore(backgroundLoop, "backgroundLoop", 1000 /*stack*/, NULL /*input*/, 0 /*prio*/,
                          &Task1 /*handle*/, 0);
}

void loop() {
  hal->checkButtons();
  // fadeIn->run(hal);
  // printDebug->run(hal);
  watchFace->run(hal);

  hal->flushCanvas();
}
