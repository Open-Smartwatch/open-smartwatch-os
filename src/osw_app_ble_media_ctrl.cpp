
#include "osw_app_ble_media_ctrl.h"

#include <BleKeyboard.h>
#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

BleKeyboard bleKeyboard = BleKeyboard("osw", "p3dt", 100);

class VoidPainter : public DrawPixel {
 public:
  VoidPainter(){};
  void drawPixel(int32_t x, int32_t y, uint16_t color){
      // nop
  };
};

void OswAppBLEMEdiaCtrl::setup(OswHal* hal) {
  // VoidPainter* voidPainter = new VoidPainter();
  // hal->getCanvas()->getGraphics2D()->disableBuffer(voidPainter);
  hal->disableDisplayBuffer();
  bleKeyboard.begin();
}

void OswAppBLEMEdiaCtrl::loop(OswHal* hal) {
  static long lastDraw = 0;
  static bool fillScreen = true;
  Serial.println(ESP.getFreeHeap());

  if (hal->btn3Down()) {
    bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
  } else if (hal->btn2Down()) {
    bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
  }

  if (millis() - lastDraw > 250 /* 4fps redraw */) {
    lastDraw = millis();

    if (fillScreen) {
      fillScreen = false;
      hal->getCanvas()->getGraphics2D()->fill(rgb565(0, 0, 0));
    }
    
    hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
    hal->getCanvas()->setTextSize(2);

    if (bleKeyboard.isConnected()) {
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

    hal->flushCanvas();
  }

  if (hal->btn1Down()) {
    // deep sleep after 30 seconds
    Serial.println("good night");
    hal->getCanvas()->getGraphics2D()->fill(rgb565(0, 0, 0));
    hal->flushCanvas();
    hal->setBrightness(10);
    delay(500);
    hal->deepSleep();  // 1h
  }
}