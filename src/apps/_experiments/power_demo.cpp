
#include "./apps/_experiments/power_demo.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

void drawColors() {
  for (uint8_t x = 0; x < 240; x++) {
    for (uint8_t y = 0; y < 240; y++) {
      OswHal::getInstance()->getCanvas()->drawPixel(x, y, rgb565(x, y, 128));
      if (y > 112 && y < 132) {
        OswHal::getInstance()->getCanvas()->drawPixel(x, y, 0);
      }
    }
  }
}

void OswAppPowerDemo::loop() {
  static long loopCount = 0;
  loopCount++;
  OswHal* hal = OswHal::getInstance();
  hal->getCanvas()->fillScreen(0);
  hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
  hal->getCanvas()->setCursor(24, 119);

  hal->setCPUClock(240);
  hal->setBrightness(255);
  hal->getCanvas()->setCursor(40, 120);
  drawColors();
  hal->getCanvas()->print("100% Brightness / 240MHz");
  hal->flushCanvas();
  delay(3000);

  hal->setBrightness(255 * .75);
  hal->getCanvas()->setCursor(40, 120);
  drawColors();
  hal->getCanvas()->print(" 75% Brightness / 240MHz");
  hal->flushCanvas();
  delay(3000);

  hal->setBrightness(255 * .5);
  hal->getCanvas()->setCursor(40, 120);
  drawColors();
  hal->getCanvas()->print(" 50% Brightness / 240MHz");
  hal->flushCanvas();
  delay(3000);

  hal->setBrightness(255 * .25);
  hal->getCanvas()->setCursor(40, 120);
  drawColors();
  hal->getCanvas()->print(" 25% Brightness / 240MHz");
  hal->flushCanvas();
  delay(3000);

  hal->setBrightness(255 * .10);
  hal->getCanvas()->setCursor(40, 120);
  drawColors();
  hal->getCanvas()->print(" 10% Brightness / 240MHz");
  hal->flushCanvas();
  delay(3000);

  hal->setCPUClock(160);
  hal->getCanvas()->setCursor(40, 120);
  drawColors();
  hal->getCanvas()->print(" 10% Brightness / 160MHz");
  hal->flushCanvas();
  delay(3000);

  hal->setCPUClock(80);
  hal->getCanvas()->setCursor(40, 120);
  drawColors();
  hal->getCanvas()->print(" 10% Brightness / 80MHz ");
  hal->flushCanvas();
  delay(3000);

  hal->setCPUClock(40);
  hal->getCanvas()->setCursor(40, 120);
  drawColors();
  hal->getCanvas()->print(" 10% Brightness / 40MHz ");
  hal->flushCanvas();
  delay(3000);

  hal->setCPUClock(20);
  hal->getCanvas()->setCursor(40, 120);
  drawColors();
  hal->getCanvas()->print(" 10% Brightness / 20MHz ");
  hal->flushCanvas();
  delay(3000);

  hal->setCPUClock(10);
  hal->getCanvas()->setCursor(40, 120);
  drawColors();
  hal->getCanvas()->print(" 10% Brightness / 10MHz ");
  hal->flushCanvas();
  delay(3000);

  hal->getCanvas()->setCursor(40, 120);
  drawColors();
  hal->getCanvas()->print(" going to deep sleep    ");
  hal->flushCanvas();
  delay(3000);
}
