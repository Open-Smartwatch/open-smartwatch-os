
#include "./apps/tools/time_from_web.h"

#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <time.h>

#include "osw_ui_util.h"

void OswAppTimeFromWeb::setup(OswHal* hal) { hal->getWiFi()->setDebugStream(&Serial); }

void OswAppTimeFromWeb::loop(OswHal* hal) {
  hal->gfx()->fill(rgb565(0, 0, 0));
  hal->gfx()->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));
  hal->gfx()->setTextSize(2);

  hal->gfx()->setTextCursorBtn3();
  if (hal->getWiFi()->isConnected()) {
    hal->gfx()->print(LANG_DISCONNECT);
  } else {
    hal->gfx()->print(LANG_CONNECT);
  }

  if (hal->btnHasGoneDown(BUTTON_3)) {
    if (hal->getWiFi()->isConnected()) {
      hal->getWiFi()->disableWiFi();
    } else {
      hal->getWiFi()->joinWifi();
    }
  }

  if (hal->getWiFi()->isConnected()) {
    hal->gfx()->setTextCursorBtn2();
    hal->gfx()->print(LANG_TFW_UPDATE);
    if (hal->btnHasGoneDown(BUTTON_2)) {
      if (hal->getWiFi()->isConnected()) {
        Serial.println("updating...");

        hal->updateTimeViaNTP(TIMEZONE * 3600, DAYLIGHTOFFSET * 3600, 5 /*seconds*/);
        Serial.println("done...");
      }
    }
  }

  hal->gfx()->setTextSize(4);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(4), 120);

  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;
  hal->getLocalTime(&hour, &minute, &second);
  hal->gfx()->printDecimal(hour, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(minute, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(second, 2);

  hal->requestFlush();
}
void OswAppTimeFromWeb::stop(OswHal* hal) {}
