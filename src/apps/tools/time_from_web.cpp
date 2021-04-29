
#include "./apps/tools/time_from_web.h"

#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <time.h>

#include "osw_ui_util.h"

void OswAppTimeFromWeb::setup(OswHal* hal) { hal->getWiFi()->setDebugStream(&Serial); }

void OswAppTimeFromWeb::loop(OswHal* hal) {
  hal->getCanvas()->getGraphics2D()->fill(rgb565(0, 0, 0));
  hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
  hal->getCanvas()->setTextSize(2);

  if (hal->getWiFi()->isConnected()) {
    hal->getCanvas()->setCursor(220 - defaultFontXOffset(10, 2), 42);
    hal->getCanvas()->print("Disconnect");
  } else {
    hal->getCanvas()->setCursor(220 - defaultFontXOffset(6, 2), 42);
    hal->getCanvas()->print("Connect");
  }
  if (hal->btn3Down()) {
    if (hal->getWiFi()->isConnected()) {
      hal->getWiFi()->disableWiFi();
    } else {
      hal->getWiFi()->joinWifi();
    }
    hal->clearBtn3();
  }

  if (hal->getWiFi()->isConnected()) {
    hal->getCanvas()->setCursor(220 - defaultFontXOffset(6, 2), 182);
    hal->getCanvas()->print("Update");
    if (hal->btn2Down()) {
      if (hal->getWiFi()->isConnected()) {
        Serial.println("updating...");

        hal->updateTimeViaNTP(TIMEZONE * 3600, DAYLIGHTOFFSET * 3600, 5 /*seconds*/);
        Serial.println("done...");
      }
      hal->clearBtn2();
    }
  }

  hal->getCanvas()->setTextSize(4);
  hal->getCanvas()->setCursor(defaultFontXOffset(1, 4), 120 - defaultFontYOffset(1, 4) / 2);

  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;
  hal->getLocalTime(&hour, &minute, &second);
  print2Digits(hal, hour);
  hal->getCanvas()->print(":");
  print2Digits(hal, minute);
  hal->getCanvas()->print(":");
  print2Digits(hal, second);

  hal->requestFlush();
}
void OswAppTimeFromWeb::stop(OswHal* hal) {}
