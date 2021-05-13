
#include "./apps/tools/time_config.h"

#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <time.h>

#include "osw_ui_util.h"

void OswAppTimeConfig::setup(OswHal* hal) {
  hal->getWiFi()->setDebugStream(&Serial);
  timeZone = OswConfigAllKeys::timeZone.get();
  daylightOffset = OswConfigAllKeys::daylightOffset.get();
}

void OswAppTimeConfig::enterManualMode(OswHal* hal) {
  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;
  uint32_t day = 0;
  uint32_t month = 0;
  uint32_t year = 0;
  hal->getLocalTime(&hour, &minute, &second);
  hal->getDate(&day, &month, &year);
  manualSettingTimestamp[0] = year % 10;
  manualSettingTimestamp[1] = month / 10;
  manualSettingTimestamp[2] = month % 10;
  manualSettingTimestamp[3] = day / 10;
  manualSettingTimestamp[4] = day % 10;
  manualSettingTimestamp[5] = hour / 10;
  manualSettingTimestamp[6] = hour % 10;
  manualSettingTimestamp[7] = minute / 10;
  manualSettingTimestamp[8] = minute % 10;
  manualSettingTimestamp[9] = second / 10;
  manualSettingTimestamp[10] = second % 10;
  manualSettingScreen = true;
}

void OswAppTimeConfig::loop(OswHal* hal) {
  // TODO: load from settings
  uint16_t colorActive = rgb565(255, 0, 0);
  uint16_t colorForeground = rgb565(255, 255, 255);
  uint16_t colorBackground = rgb565(0, 0, 0);

  if (!manualSettingScreen) {
    hal->gfx()->fill(colorBackground);
    hal->gfx()->setTextColor(colorForeground, colorBackground);
    hal->gfx()->setTextSize(2);

    hal->gfx()->setTextCursorBtn3();
    if (hal->getWiFi()->isConnected()) {
      hal->gfx()->print(LANG_DISCONNECT);
    } else {
      hal->gfx()->print(LANG_CONNECT);
      hal->gfx()->setTextCursorBtn2();
      hal->gfx()->print(LANG_MANUALLY);
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
          hal->updateTimeViaNTP(timeZone * 3600, daylightOffset * 3600, 5 /*seconds*/);
        }
      }
    } else {
      if (hal->btnHasGoneDown(BUTTON_2)) {
        enterManualMode(hal);
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

  } else {
    hal->gfx()->fill(colorBackground);
    hal->gfx()->setTextColor(colorForeground, colorBackground);
    hal->gfx()->setTextSize(2);

    // Add-Button
    hal->gfx()->setTextCursorBtn3();
    hal->gfx()->print("+");
    if (manualSettingStep == 12) {  // SAVE
      if (hal->btnHasGoneDown(BUTTON_3)) {
        // Date
        int16_t yy = 2020 + manualSettingTimestamp[0];
        int8_t mm = manualSettingTimestamp[1] * 10 + manualSettingTimestamp[2] - 1;  // January = 0
        int8_t dd = manualSettingTimestamp[3] * 10 + manualSettingTimestamp[4];
        int8_t h = manualSettingTimestamp[5] * 10 + manualSettingTimestamp[6];
        int8_t m = manualSettingTimestamp[7] * 10 + manualSettingTimestamp[8];
        int8_t s = manualSettingTimestamp[9] * 10 + manualSettingTimestamp[10];
        struct tm date = {s, m, h, dd, mm, yy - 1900};
        time_t epoch = mktime(&date);

        hal->setUTCTime(epoch - (timeZone * 3600) - (daylightOffset * 3600));
        manualSettingScreen = false;
      }
    } else if (manualSettingStep == 11) {  // CANCEL
      if (hal->btnHasGoneDown(BUTTON_3)) {
        manualSettingScreen = false;
      }
    } else {  // +1
      if (hal->btnHasGoneDown(BUTTON_3)) {
        manualSettingTimestamp[manualSettingStep]++;

        if (manualSettingStep == 1) {  // MONTHTEN
          if (manualSettingTimestamp[manualSettingStep] > 1) {
            manualSettingTimestamp[manualSettingStep] = 0;
          }
        } else if (manualSettingStep == 3) {  // DAYTEN
          if (manualSettingTimestamp[manualSettingStep] > 3) {
            manualSettingTimestamp[manualSettingStep] = 0;
          }
        } else if (manualSettingStep == 5) {  // HOURTEN
          if (manualSettingTimestamp[manualSettingStep] > 2) {
            manualSettingTimestamp[manualSettingStep] = 0;
          }
        } else if (manualSettingStep == 7 || manualSettingStep == 9) {  // MINTEN or SECTEN
          if (manualSettingTimestamp[manualSettingStep] > 5) {
            manualSettingTimestamp[manualSettingStep] = 0;
          }
        } else {
          if (manualSettingTimestamp[manualSettingStep] > 9) {  // other
            manualSettingTimestamp[manualSettingStep] = 0;
          }
        }
      }
    }

    // Next-Button
    hal->gfx()->setTextCursorBtn1();
    hal->gfx()->print(">");
    if (hal->btnHasGoneDown(BUTTON_1)) {
      manualSettingStep++;
      if (manualSettingStep > 12) {
        manualSettingStep = 0;
      }
    }

    hal->gfx()->setTextColor(colorForeground, colorBackground);
    hal->gfx()->setTextSize(3);
    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextLeftAligned();

    // Date
    hal->gfx()->setTextCursor((DISP_W / 2) - hal->gfx()->getTextOfsetColumns(5), DISP_H * 3 / 8);
    hal->gfx()->print("202");
    for (int8_t i = 0; i < 5; i++) {
      if (i == manualSettingStep) {
        hal->gfx()->setTextColor(colorActive, colorBackground);
      } else {
        hal->gfx()->setTextColor(colorForeground, colorBackground);
      }
      hal->gfx()->print(manualSettingTimestamp[i]);
      if (i == 0 || i == 2) {
        hal->gfx()->setTextColor(colorForeground, colorBackground);
        hal->gfx()->print("-");
      }
    }

    // Time
    hal->gfx()->setTextCursor((DISP_W / 2) - hal->gfx()->getTextOfsetColumns(4), DISP_H / 2);
    for (int8_t i = 5; i < 11; i++) {
      if (i == manualSettingStep) {
        hal->gfx()->setTextColor(colorActive, colorBackground);
      } else {
        hal->gfx()->setTextColor(colorForeground, colorBackground);
      }
      hal->gfx()->print(manualSettingTimestamp[i]);
      if (i == 6 || i == 8) {
        hal->gfx()->setTextColor(colorForeground, colorBackground);
        hal->gfx()->print(":");
      }
    }

    hal->gfx()->setTextSize(2);

    // Cancel-Field
    hal->gfx()->setTextRightAligned();
    hal->gfx()->setTextCursor((DISP_W / 2) - hal->gfx()->getTextOfsetColumns(0.5), DISP_H * 5 / 8);
    if (manualSettingStep == 11) {
      hal->gfx()->setTextColor(colorActive, colorBackground);
    } else {
      hal->gfx()->setTextColor(colorForeground, colorBackground);
    }
    hal->gfx()->print(LANG_CANCEL);

    // Done-Field
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextCursor((DISP_W / 2) + hal->gfx()->getTextOfsetColumns(0.5), DISP_H * 5 / 8);
    if (manualSettingStep == 12) {
      hal->gfx()->setTextColor(colorActive, colorBackground);
    } else {
      hal->gfx()->setTextColor(colorForeground, colorBackground);
    }
    hal->gfx()->print(LANG_SAVE);
  }

  hal->requestFlush();
}
void OswAppTimeConfig::stop(OswHal* hal) {}
