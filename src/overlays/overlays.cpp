#include "./overlays/overlays.h"

#include "osw_pins.h"
#include "osw_ui_util.h"

void drawUsbConnected(OswHal* hal, uint16_t x, uint16_t y) {
  hal->getCanvas()->getGraphics2D()->fillFrame(x, y + 4, 13, 2, rgb565(128, 128, 128));       // cable dot
  hal->getCanvas()->getGraphics2D()->fillFrame(x + 13, y + 2, 3, 6, rgb565(200, 200, 200));   // cable to casing
  hal->getCanvas()->getGraphics2D()->fillFrame(x + 20, y + 2, 11, 6, rgb565(128, 128, 128));  // connector
  hal->getCanvas()->getGraphics2D()->fillFrame(x + 16, y, 8, 10, rgb565(200, 200, 200));      // casing
}

void drawBattery(OswHal* hal, uint16_t x, uint16_t y) {
  hal->getCanvas()->getGraphics2D()->drawFrame(x, y, 28, 12, rgb565(255, 255, 255));         // outer frame
  hal->getCanvas()->getGraphics2D()->drawFrame(x + 28, y + 3, 3, 6, rgb565(255, 255, 255));  // tip

  hal->getCanvas()->getGraphics2D()->fillFrame(x + 2, y + 2, 25 * (hal->getBatteryPercent() / 100.0), 9,
                                               rgb565(255, 255, 255));  // outer frame
}

void drawWiFi(OswHal* hal, uint16_t x, uint16_t y) {
  if (hal->getWiFi()->isEnabled()) {
    for (uint8_t i = 0; i < hal->getWiFi()->getSignalQuality() / 20; i++) {
      hal->getCanvas()->getGraphics2D()->fillFrame(x + 3 * i, y + 12 - i * 2, 2, i * 2,
                                                   rgb565(255, 255, 255));  // outer frame
    }
  }
}

void drawOverlays(OswHal* hal) {
  if (hal->isCharging()) {
    drawUsbConnected(hal, 120 - 16, 6);  // width is 31
  } else if (!hal->getWiFi()->isConnected()) {
    drawBattery(hal, 120 - 15, 6);
    // hal->getCanvas()->setTextSize(1);
    // hal->getCanvas()->setCursor(120 - defaultFontXOffset(1, 1), 20);
    // hal->getCanvas()->print(hal->getBatteryRaw());
  }
  drawWiFi(hal, 138, 6);

  // draw app switcher icon and
  if (hal->btnIsDownSince(BUTTON_1)) {
    if (hal->btnIsDownSince(BUTTON_1) > BTN_1_APP_SWITCH_TIMEOUT - 255) {
      uint8_t brightness = hal->btnIsDownSince(BUTTON_1) - 255;
      hal->gfx()->drawFrame(120 - 4, 230 - 4, 8, 8, rgb565(brightness, brightness, brightness));
    }
    if (hal->btnIsDownSince(BUTTON_1) > BTN_1_APP_SWITCH_TIMEOUT) {
      hal->gfx()->fillFrame(120 - 4, 230 - 4, 8, 8, rgb565(255, 255, 255));
    }
    if (hal->btnIsDownSince(BUTTON_1) > BTN_1_SLEEP_TIMEOUT) {
      hal->gfx()->fillCircle(120, 230, 9, rgb565(128, 128, 128));
      hal->gfx()->fillCircle(120, 230, 8, rgb565(255, 255, 255));
      hal->gfx()->fillCircle(123, 230, 6, rgb565(0, 0, 0));
    }
  }
}
