
#include "./apps/tools/water_level.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_pins.h>

const int middleX = 120;
const int middleY = 120;
const int screenWidth = 240;

void OswAppWaterLevel::setup(OswHal* hal) {}

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

void OswAppWaterLevel::circlesDisplay(OswHal* hal) {
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
}

void OswAppWaterLevel::drawBar(OswHal* hal, const float value, char text, const int x) {
  Graphics2D* gfx = hal->getCanvas()->getGraphics2D();

  const int fontHeight = 1;
  hal->getCanvas()->setTextSize(fontHeight);

  const int maxVal = 2;
  const int redComponent = min(255, max(0, (int)((2 + value) / (2 * maxVal) * 255)));
  const int greenComponent = 255 - min(255, max(0, (int)((2 + value) / (2 * maxVal) * 255)));

  const int width = 10;
  const int height = 10;

  const int barHeight = min(100, (int)(32 * abs(value) / maxVal * 2));
  const int yOffset = value <= 0 ? barHeight : 0;

  gfx->fillRFrame(x, 120 - 5 - yOffset, width, height + barHeight, 5, rgb565(redComponent, greenComponent, 0));

  bool isMiddleValue = value > -0.25 && value < 0.25;

  const int backgroundColor = isMiddleValue ? rgb565(240, 240, 0) : 0;
  const int foregroundColor = isMiddleValue ? 0 : WHITE;

  gfx->fillCircle(x + 4, 120 + 2, width / 2 + 3, backgroundColor);

  gfx->drawCircle(x + 4, 120 + 2, width / 2 + 3, WHITE);

  hal->getCanvas()->setCursor(x + 2, 120 - 2);

  hal->getCanvas()->setTextColor(foregroundColor, backgroundColor);
  hal->getCanvas()->print(text);
}

void OswAppWaterLevel::barsDisplay(OswHal* hal) {
  const float xValue = hal->getAccelerationX();
  const float yValue = hal->getAccelerationY();
  const float zValue = hal->getAccelerationZ();

  try {
    drawBar(hal, xValue, 'X', 80);
    drawBar(hal, yValue, 'Y', 120);
    drawBar(hal, zValue, 'Z', 160);

  } catch (const std::exception& e) {
    hal->getCanvas()->print(e.what());
  }
}
int displayMode = 1;
void OswAppWaterLevel::loop(OswHal* hal) {
  hal->getCanvas()->fillScreen(BLACK);

  // to better understand the accelerometer values use the debug function
  // debug(hal);

  if (hal->btn2Down() >= BTN_1_SLEEP_TIMEOUT) {
    displayMode = (displayMode + 1) % 2;
  }
  switch (displayMode) {
    case 0:
      barsDisplay(hal);
      break;

    default:
      circlesDisplay(hal);
      break;
  }

  hal->requestFlush();
}

void OswAppWaterLevel::stop(OswHal* hal) {}
