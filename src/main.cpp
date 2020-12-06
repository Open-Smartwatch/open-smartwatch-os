#include <Arduino.h>
#include <osw_app.h>
#include <osw_app_autumn.h>
#include <osw_app_fadein_display.h>
#include <osw_app_print_debug.h>
#include <osw_app_show_display_size.h>
#include <osw_app_watchface_demo.h>
#include <osw_hal.h>

OswHal *hal = new OswHal();
OswAppPrintDebug *printDebug = new OswAppPrintDebug();
OswAppFadeInDisplay *fadeIn = new OswAppFadeInDisplay(4 * 255);
// OswAppWatchFaceDemo *watchFace = new OswAppWatchFaceDemo();
OswAppAutumn *autumn = new OswAppAutumn();
OswAppShowDisplaySize *displaySize = new OswAppShowDisplaySize();

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

  hal->setBrightness(255);
  hal->gpsFullOnGpsGlonassBeidu();

  xTaskCreatePinnedToCore(backgroundLoop, "backgroundLoop", 1000 /*stack*/, NULL /*input*/, 0 /*prio*/,
                          &Task1 /*handle*/, 0);

  // call your watch app setup here
  // autumn->setup(hal);
}

void loop() {
  hal->checkButtons();

  // call your watch app loop here

  fadeIn->loop(hal);
  // printDebug->loop(hal);
  // autumn->loop(hal);

  displaySize->loop(hal);

  hal->flushCanvas();
}
