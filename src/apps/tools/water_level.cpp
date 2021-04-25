
#include "./apps/tools/water_level.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

const int middleX = 120;
const int middleY = 120;
const int screenWidth = 240;

void OswAppWaterLevel::setup(OswHal* hal) {
  // hal->enableDisplayBuffer();
}

void OswAppWaterLevel::debug(OswHal* hal) {
  const int defaultXHint = 40;
  const int defaultYHint = 40;
  const int fontHeight = 1;
  const int lineHeight = fontHeight * 10 + 2;

  hal->getCanvas()->setTextSize(fontHeight);

  hal->getCanvas()->setTextColor(WHITE);
  hal->getCanvas()->setCursor(defaultXHint, defaultYHint + 0 * lineHeight);
  hal->getCanvas()->print("Accelerometer Data:");

  hal->getCanvas()->setCursor(defaultXHint, defaultYHint + 1 * lineHeight);
  hal->getCanvas()->print("X: ");
  hal->getCanvas()->print(hal->getAccelerationX());

  hal->getCanvas()->setCursor(defaultXHint, defaultYHint + 2 * lineHeight);
  hal->getCanvas()->print("Y: ");
  hal->getCanvas()->print(hal->getAccelerationY());

  hal->getCanvas()->setCursor(defaultXHint, defaultYHint + 3 * lineHeight);
  hal->getCanvas()->print("Z: ");
  hal->getCanvas()->print(hal->getAccelerationZ());
}

void OswAppWaterLevel::loop(OswHal* hal) {
  hal->getCanvas()->fillScreen(BLACK);

  // to better understand the accelerometer values use the debug function
  // debug(hal);

  Graphics2D* gfx = hal->getCanvas()->getGraphics2D();

  const float xValue = hal->getAccelerationX();
  const float yValue = hal->getAccelerationY();
  const float zValue = hal->getAccelerationZ();

  const bool isXYAccelerationInMiddle = abs(yValue) < 0.25 && abs(xValue) < 0.25;

  uint16_t color = isXYAccelerationInMiddle ? GREEN : WHITE;

  hal->getCanvas()->drawFastHLine(0, middleY, screenWidth, color);
  hal->getCanvas()->drawFastVLine(middleX, 0, screenWidth, color);

  const int x0 = middleX + xValue * 64;
  const int y0 = middleY - yValue * 64;
  const int defaultRadius = 32;
  const int bigCircleMargin = 4;

  const int redZComponent = min(255, max(0, (int)((2 + zValue) / 4 * 255)));
  const int greenZComponent = 255 - min(255, max(0, (int)((2 + zValue) / 4 * 255)));

  gfx->fillCircle(x0, y0, defaultRadius + bigCircleMargin, rgb565(0, redZComponent, greenZComponent));
  gfx->fillCircle(x0, y0, defaultRadius, color);
  gfx->drawCircle(x0, y0, defaultRadius + bigCircleMargin, BLACK);
  gfx->drawCircle(x0, y0, defaultRadius, BLACK);

  hal->requestFlush();
}

void OswAppWaterLevel::stop(OswHal* hal) {  // hal->disableDisplayBuffer();
}
