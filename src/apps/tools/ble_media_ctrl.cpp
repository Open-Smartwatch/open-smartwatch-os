
#include "./apps/tools/ble_media_ctrl.h"

#include <BleKeyboard.h>
#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

BleKeyboard* bleKeyboard;

void OswAppBLEMEdiaCtrl::setup(OswHal* hal) {
  hal->disableDisplayBuffer();
  bleKeyboard = new BleKeyboard("osw", "p3dt", 100);
  bleKeyboard->begin();
}

void OswAppBLEMEdiaCtrl::loop(OswHal* hal) {

  static long lastDraw = 0;
  static bool fillScreen = true;
  Serial.println(ESP.getFreeHeap());

  if (hal->btn3Down()) {
    bleKeyboard->write(KEY_MEDIA_VOLUME_UP);
  } else if (hal->btn2Down()) {
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
      hal->getCanvas()->setCursor(100, 50);
      if (hal->btn3Down()) {
        hal->getCanvas()->print("Volume (+)");
      } else {
        hal->getCanvas()->print("Volume  + ");
      }
      hal->getCanvas()->setCursor(100, 190);
      if (hal->btn2Down()) {
        hal->getCanvas()->print("Volume (-)");
      } else {
        hal->getCanvas()->print("Volume  - ");
      }
    } else {
      hal->getCanvas()->setCursor(20, 120);
      hal->getCanvas()->print("Connecting...");
    }

    hal->requestFlush();
  }
}

void OswAppBLEMEdiaCtrl::stop(OswHal* hal) {
  bleKeyboard->end();
  delete bleKeyboard;
  hal->enableDisplayBuffer();
}