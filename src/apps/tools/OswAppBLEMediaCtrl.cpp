#ifndef OSW_EMULATOR
#ifdef OSW_FEATURE_BLE_MEDIA_CTRL
#include "./apps/tools/OswAppBLEMediaCtrl.h"

#include <config.h>
#include <gfx_util.h>
#include <OswAppV1.h>
#include <osw_hal.h>

// workaround for compile issues via https://github.com/espressif/arduino-esp32/issues/6388
#define BLE_42_FEATURE_SUPPORT TRUE
#define BLE_50_FEATURE_SUPPORT TRUE
#include <BleKeyboard.h>

BleKeyboard* bleKeyboard;

void OswAppBLEMediaCtrl::setup() {
    OswHal::getInstance()->disableDisplayBuffer();
    bleKeyboard = new BleKeyboard(BLE_DEVICE_NAME, "p3dt", 100);
    bleKeyboard->begin();
}

void OswAppBLEMediaCtrl::loop() {
    static long lastDraw = 0;
    static bool fillScreen = true;
    OSW_LOG_D(ESP.getFreeHeap());

    OswHal* hal = OswHal::getInstance();
    if (hal->btnHasGoneDown(BUTTON_2) && hal->btnHasGoneDown(BUTTON_3)) {
        bleKeyboard->write(KEY_MEDIA_PLAY_PAUSE);
    } else if (hal->btnHasGoneDown(BUTTON_3)) {
        bleKeyboard->write(KEY_MEDIA_VOLUME_UP);
    } else if (hal->btnHasGoneDown(BUTTON_2)) {
        bleKeyboard->write(KEY_MEDIA_VOLUME_DOWN);
    }

    if (millis() - lastDraw > 250 /* 4fps redraw */) {
        lastDraw = millis();

        if (fillScreen) {
            fillScreen = false;
            hal->getCanvas()->fill(rgb565(0, 0, 0));
        }

        hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
        hal->getCanvas()->setTextSize(2);

        if (bleKeyboard->isConnected()) {
            hal->getCanvas()->setTextCursor(20, 130);
            hal->getCanvas()->print(LANG_CONNECTED);
            hal->getCanvas()->setTextCursor(100, 50);
            hal->getCanvas()->print(LANG_BMC_VOLUME);
            hal->getCanvas()->print("  + ");

            hal->getCanvas()->setTextCursor(100, 190);
            hal->getCanvas()->print(LANG_BMC_VOLUME);
            hal->getCanvas()->print("  - ");

        } else {
            hal->getCanvas()->setTextCursor(20, 110);
            hal->getCanvas()->print(LANG_BMC_CONNECTING);
        }
    }
}

void OswAppBLEMediaCtrl::stop() {
    bleKeyboard->end();
    delete bleKeyboard;
    OswHal::getInstance()->enableDisplayBuffer();
}
#endif
#endif
