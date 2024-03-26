#include "./overlays/overlays.h"

#include <services/OswServiceTaskMemMonitor.h>
#include <services/OswServiceTaskWiFi.h>
#include <services/OswServiceTasks.h>

#include "osw_ui.h"

void drawUsbConnected(uint16_t x, uint16_t y) {
    Graphics2DPrint* gfx = OswHal::getInstance()->gfx();
    gfx->fillFrame(x, y + 4, 13, 2, OswUI::getInstance()->getForegroundColor());  // cable dot
    gfx->fillFrame(x + 13, y + 2, 3, 6, OswUI::getInstance()->getForegroundColor());  // cable to casing
    gfx->fillFrame(x + 20, y + 2, 11, 6, OswUI::getInstance()->getForegroundColor());  // connector
    gfx->fillFrame(x + 16, y, 8, 10, OswUI::getInstance()->getForegroundColor());  // casing
}

void drawBattery(uint16_t x, uint16_t y) {
    Graphics2DPrint* gfx = OswHal::getInstance()->gfx();
    gfx->drawFrame(x, y, 28, 12,
                   OswUI::getInstance()->getForegroundColor());  // outer frame
    gfx->drawFrame(x + 28, y + 3, 3, 6, OswUI::getInstance()->getForegroundColor());  // tip

    const uint8_t batLvl = OswHal::getInstance()->getBatteryPercent();

    uint16_t batColor = OswUI::getInstance()->getSuccessColor();
    batColor = batLvl < 50 ? OswUI::getInstance()->getWarningColor() : batColor;
    batColor = batLvl < 25 ? OswUI::getInstance()->getDangerColor() : batColor;

    if (batLvl < 0.5f) {
        // This happens initial discharging (calibration phase) of the battery or when you're in trouble!
        gfx->fillFrame(x + 2, y + 2, 25, 9, OswUI::getInstance()->getInfoColor());
    } else {
        gfx->fillFrame(x + 2, y + 2, 25 * (batLvl / 100.0f), 9, batColor);  // charge
    }
}

#ifdef OSW_FEATURE_WIFI
void drawWiFi(uint16_t x, uint16_t y) {
    if (OswServiceAllTasks::wifi.isWiFiEnabled()) {
        for (uint8_t i = 0; i < OswServiceAllTasks::wifi.getSignalQuality() / 20; i++) {
            OswHal::getInstance()->gfx()->fillFrame(x + 3 * i, y + 12 - i * 2, 2, i * 2, OswUI::getInstance()->getForegroundColor());  // outer frame
        }
    }
}
#endif

#ifndef OSW_EMULATOR
void drawLowMemory(uint16_t x, uint16_t y) {
    if (!OswServiceAllTasks::memory.hasLowMemoryCondition())
        return;
    Graphics2DPrint* gfx = OswHal::getInstance()->gfx();
    gfx->fillFrame(x + 2, y + 2, 11, 11, OswUI::getInstance()->getDangerColor());
    gfx->drawLine(x, y + 3, x + 14, y + 3, OswUI::getInstance()->getDangerColor());
    gfx->drawLine(x, y + 7, x + 14, y + 7, OswUI::getInstance()->getDangerColor());
    gfx->drawLine(x, y + 11, x + 14, y + 11, OswUI::getInstance()->getDangerColor());
    gfx->drawLine(x + 3, y, x + 3, y + 14, OswUI::getInstance()->getDangerColor());
    gfx->drawLine(x + 7, y, x + 7, y + 14, OswUI::getInstance()->getDangerColor());
    gfx->drawLine(x + 11, y, x + 11, y + 14, OswUI::getInstance()->getDangerColor());
}
#endif

void drawOverlays() {
    bool drawBat = true;
#ifdef OSW_FEATURE_WIFI
    // IF we have wifi enabled, we have to consider an additional condition to check
    drawBat = !OswServiceAllTasks::wifi.isEnabled();
#endif
#ifndef OSW_EMULATOR
    drawLowMemory(84, 4);
#endif

    if (OswHal::getInstance()->isCharging())
        drawUsbConnected(120 - 16, 6);  // width is 31
    else if (drawBat)
        drawBattery(120 - 15, 6);

#ifdef OSW_FEATURE_WIFI
    drawWiFi(138, 6);
#endif
}
