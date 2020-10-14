#include <Arduino.h>
#include <gfx_util.h>
#include <osw_hal.h>
#include <osw_pins.h>

#define MIN_LAYER 0
#define MAX_LAYER 6

OswHal hal;

void setup() {
  Serial.begin(115200);

  hal.setupPower();
  hal.setupButtons();
  hal.setupSensors();
  hal.setupDisplay();
  hal.setupGps();

  hal.setBrightness(255);
}

void loop() {
  static long loopCount = 0;
  loopCount++;
  hal.checkButtons();
  hal.getCanvas()->fillScreen(0);
  hal.getCanvas()->setTextColor(rgb565(255, 255, 255));
  hal.getCanvas()->setCursor(24, 119);
  hal.getCanvas()->print("Hello World ");
  hal.getCanvas()->print(loopCount);
  hal.flushCanvas();
}