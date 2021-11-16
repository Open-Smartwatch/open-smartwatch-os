
#include "./apps/tools/ble_media_ctrl.h"

#include <BleKeyboard.h>
#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

BleKeyboard* bleKeyboard;

void OswAppBLEMEdiaCtrl::setup() {
  OswHal::getInstance()->disableDisplayBuffer();
  bleKeyboard = new BleKeyboard(BLE_DEVICE_NAME, "p3dt", 100);
  bleKeyboard->begin();
}

void OswAppBLEMEdiaCtrl::loop() {
  static long lastDraw = 0;
  static bool fillScreen = true;
  Serial.println(ESP.getFreeHeap());

  OswHal* hal = OswHal::getInstance();
  if (hal->btnHasGoneDown(BUTTON_3)) {
    bleKeyboard->write(KEY_MEDIA_VOLUME_UP);
  } else if (hal->btnHasGoneDown(BUTTON_2)) {
    bleKeyboard->write(KEY_MEDIA_VOLUME_DOWN);
  }

  if (millis() - lastDraw > 250 /* 4fps redraw */) {
    lastDraw = millis();

    if (fillScreen) {
      fillScreen = false;
      hal->getCanvas()->getGraphics2D()->fill(rgb565(0, 0, 0));
    }

    hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
    hal->getCanvas()->setTextSize(2);

    if (bleKeyboard->isConnected()) {
      hal->getCanvas()->setCursor(20, 130);
      hal->getCanvas()->print(LANG_CONNECTED);
      hal->getCanvas()->setCursor(100, 50);
      hal->getCanvas()->print(LANG_BMC_VOLUME);
      hal->getCanvas()->print("  + ");

      hal->getCanvas()->setCursor(100, 190);
      hal->getCanvas()->print(LANG_BMC_VOLUME);
      hal->getCanvas()->print("  - ");

    } else {
      hal->getCanvas()->setCursor(20, 110);
      hal->getCanvas()->print(LANG_BMC_CONNECTING);
    }

    hal->requestFlush();
  }
}

void OswAppBLEMEdiaCtrl::stop() {
  bleKeyboard->end();
  delete bleKeyboard;
  OswHal::getInstance()->enableDisplayBuffer();
}
