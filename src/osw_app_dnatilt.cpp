
#include "osw_app_dnatilt.h"

#include <config.h>
#include <gfx_util.h>
#include <mini-wifi.h>
#include <osw_app.h>
#include <osw_hal.h>

#define HTTP_GET_BUF_LEN 10
char httpGetBuffer[HTTP_GET_BUF_LEN];
MiniWifi wifi(MINI_IOT_DEVICENAME, WIFI_SSID, WIFI_PASS);

void OswAppDNATilt::setup(OswHal* hal) {  //
  wifi.setDebugStream(&Serial);
}

void OswAppDNATilt::loop(OswHal* hal) {
  wifi.checkWifi();

  static long lastDraw = 0;

  if (millis() - lastDraw > 250 /* 4fps redraw */) {
    lastDraw = millis();
    hal->getCanvas()->getGraphics2D()->fill(rgb565(0, 0, 0));
    hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
    hal->getCanvas()->setCursor(20, 100);
    hal->getCanvas()->setTextSize(2);

    if (hal->getAccelerationX() > 250) {
      hal->getCanvas()->print("DNA Lamp ++");
    } else if (hal->getAccelerationX() < -250) {
      hal->getCanvas()->print("DNA Lamp --");
    } else {
      if (wifi.isConnected()) {
        hal->getCanvas()->print("DNA Lamp");
      } else {
        hal->getCanvas()->print("No WiFi");
      }
    }

    hal->flushCanvas();
  }

  if (hal->getAccelerationX() > 250) {
    wifi.get("http://192.168.1.54/api/ladder/hue/decr", httpGetBuffer, HTTP_GET_BUF_LEN);
  } else if (hal->getAccelerationX() < -250) {
    wifi.get("http://192.168.1.54/api/ladder/hue/incr", httpGetBuffer, HTTP_GET_BUF_LEN);
  }

  if (hal->btn3Down()) {
    // deep sleep after 30 seconds
    Serial.println("good night");
    hal->getCanvas()->getGraphics2D()->fill(rgb565(0, 0, 0));
    hal->flushCanvas();
    hal->setBrightness(10);
    delay(500);
    hal->deepSleep();  // 1h
  }
}
