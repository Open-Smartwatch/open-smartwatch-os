
#include "./apps/tools/water_level.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

void OswAppWaterLevel::setup(OswHal* hal) {
  // hal->enableDisplayBuffer();
}

void OswAppWaterLevel::loop(OswHal* hal) {
  hal->getCanvas()->fillScreen(rgb565(200, 200, 200));
  // hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
  // hal->getCanvas()->setCursor(40, 120);
  // hal->getCanvas()->print("Accelerometer Data:");

  // hal->getCanvas()->setCursor(40, 130);
  // hal->getCanvas()->print("X: ");
  // hal->getCanvas()->print(hal->getAccelerationX());

  // hal->getCanvas()->setCursor(40, 140);
  // hal->getCanvas()->print("Y: ");
  // hal->getCanvas()->print(hal->getAccelerationY());

  // hal->getCanvas()->setCursor(40, 150);
  // hal->getCanvas()->print("Z: ");
  // hal->getCanvas()->print(hal->getAccelerationZ());

  // draw crosshair
  hal->getCanvas()->drawFastHLine(0, 120, 240, rgb565(255, 255, 255));
  hal->getCanvas()->drawFastVLine(120, 0, 240, rgb565(255, 255, 255));

  Graphics2D* gfx = hal->getCanvas()->getGraphics2D();
  uint16_t err = min(255, (int)(abs(hal->getAccelerationX()) + abs(hal->getAccelerationY())));
  uint16_t color = rgb565(err, 255 - err, 0);
  if (abs(hal->getAccelerationY()) < 32 && abs(hal->getAccelerationX()) < 32) {
    color = rgb565(0, 255, 0);
    hal->getCanvas()->drawFastHLine(0, 120, 240, rgb565(0, 255, 0));
    hal->getCanvas()->drawFastVLine(120, 0, 240, rgb565(0, 255, 0));
  }
  gfx->fillCircle(120 - hal->getAccelerationX() / 8.0, 120 - hal->getAccelerationY() / 8.0, 32, color);
  gfx->drawCircle(120 - hal->getAccelerationX() / 8.0, 120 - hal->getAccelerationY() / 8.0, 32, rgb565(0, 0, 255));

  hal->requestFlush();
}

void OswAppWaterLevel::stop(OswHal* hal) {  // hal->disableDisplayBuffer();
}
