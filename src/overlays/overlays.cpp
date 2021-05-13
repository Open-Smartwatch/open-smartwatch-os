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
  }
  drawWiFi(hal, 138, 6);
}
