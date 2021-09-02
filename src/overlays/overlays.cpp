#include "./overlays/overlays.h"

#include <services/OswServiceTaskWiFi.h>
#include <services/OswServiceTasks.h>

#include "osw_pins.h"
#include "osw_ui.h"

void drawUsbConnected(OswHal* hal, uint16_t x, uint16_t y) {
  hal->getCanvas()->getGraphics2D()->fillFrame(x, y + 4, 13, 2,
                                               OswUI::getInstance()->getForegroundDimmedColor());  // cable dot
  hal->getCanvas()->getGraphics2D()->fillFrame(x + 13, y + 2, 3, 6,
                                               OswUI::getInstance()->getForegroundDimmedColor());  // cable to casing
  hal->getCanvas()->getGraphics2D()->fillFrame(x + 20, y + 2, 11, 6,
                                               OswUI::getInstance()->getForegroundDimmedColor());  // connector
  hal->getCanvas()->getGraphics2D()->fillFrame(x + 16, y, 8, 10,
                                               OswUI::getInstance()->getForegroundDimmedColor());  // casing
}

void drawBattery(OswHal* hal, uint16_t x, uint16_t y) {
  hal->getCanvas()->getGraphics2D()->drawFrame(x, y, 28, 12,
                                               OswUI::getInstance()->getForegroundColor());  // outer frame
  hal->getCanvas()->getGraphics2D()->drawFrame(x + 28, y + 3, 3, 6, OswUI::getInstance()->getForegroundColor());  // tip

  const uint8_t batLvl = hal->getBatteryPercent();

  uint16_t batColor = OswUI::getInstance()->getSuccessColor();
  batColor = batLvl < 50 ? OswUI::getInstance()->getWarningColor() : batColor;
  batColor = batLvl < 25 ? OswUI::getInstance()->getDangerColor() : batColor;

  hal->getCanvas()->getGraphics2D()->fillFrame(x + 2, y + 2, 25 * (batLvl / 100.0), 9,
                                               batColor);  // charge
}

#ifdef OSW_FEATURE_WIFI
void drawWiFi(OswHal* hal, uint16_t x, uint16_t y) {
  if (OswServiceAllTasks::wifi.isWiFiEnabled()) {
    for (uint8_t i = 0; i < OswServiceAllTasks::wifi.getSignalQuality() / 20; i++) {
      hal->getCanvas()->getGraphics2D()->fillFrame(x + 3 * i, y + 12 - i * 2, 2, i * 2,
                                                   OswUI::getInstance()->getForegroundColor());  // outer frame
    }
  }
}
#endif

void drawOverlays(OswHal* hal) {
  bool drawBat = true;
#ifdef OSW_FEATURE_WIFI
  // IF we have wifi enabled, we have to consider an additional condition to check
  drawBat = !OswServiceAllTasks::wifi.isConnected();
#endif

  if (hal->isCharging())
    drawUsbConnected(hal, 120 - 16, 6);  // width is 31
  else if (drawBat)
    drawBattery(hal, 120 - 15, 6);

#ifdef OSW_FEATURE_WIFI
  drawWiFi(hal, 138, 6);
#endif
}
