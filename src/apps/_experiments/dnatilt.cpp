#ifdef OSW_FEATURE_WIFI
#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
#include "./apps/_experiments/dnatilt.h"

#include <config.h>
#include <gfx_util.h>
#include <OswAppV1.h>
#include <osw_hal.h>

#include <services/OswServiceTasks.h>
#include <services/OswServiceTaskWiFi.h>

#define HTTP_GET_BUF_LEN 10
char httpGetBuffer[HTTP_GET_BUF_LEN];

void OswAppDNATilt::setup() {

}

void OswAppDNATilt::loop() {
    static long lastDraw = 0;

    OswHal* hal = OswHal::getInstance();
    if (millis() - lastDraw > 250 /* 4fps redraw */) {
        lastDraw = millis();
        hal->getCanvas()->fill(rgb565(0, 0, 0));
        hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
        hal->getCanvas()->setTextCursor(20, 100);
        hal->getCanvas()->setTextSize(2);

        if (hal->environment()->getAccelerationX() > 250) {
            hal->getCanvas()->print("DNA Lamp ++");
        } else if (hal->environment()->getAccelerationX() < -250) {
            hal->getCanvas()->print("DNA Lamp --");
        } else {
            if (OswServiceAllTasks::wifi.isConnected()) {
                hal->getCanvas()->print("DNA Lamp");
            } else {
                hal->getCanvas()->print("No WiFi");
            }
        }
    }

    /*
      THIS IS NOW MANAGED BY THE WIFI SERVICE.
      The get() method is still a NON-STATIC method of MiniWiFi and therefore currently unusable.
      Then you need this code working, please contact the author of the MiniWiFi lib or extend it yourself.

      if (hal->getAccelerationX() > 250) {
        hal->getWiFi()->get("http://192.168.1.54/api/ladder/hue/decr", httpGetBuffer, HTTP_GET_BUF_LEN);
      } else if (hal->getAccelerationX() < -250) {
        hal->getWiFi()->get("http://192.168.1.54/api/ladder/hue/incr", httpGetBuffer, HTTP_GET_BUF_LEN);
      }
    */
}
#endif
#endif
