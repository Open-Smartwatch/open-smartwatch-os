#include "./apps/main/watchface_digital.h"

#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <time.h>
#include <string>
using std::string;

#include "osw_ui_util.h"

#include "bma400_defs.h"

#define COLOR_BLACK rgb565(0, 0, 0)

void drawDate(OswHal* hal) {
    string day = "";
    uint32_t dayInt = 0;
    uint32_t monthInt = 0;
    uint32_t yearInt = 0;
    int charLen = 3;
    hal->getWeekdayString(charLen, &day);
    hal->getDate(&dayInt, &monthInt, &yearInt);

    //we want to output a value like "Wed, 05/02/2021"
    char date_Array[charLen + 1];

    strcpy(date_Array, day.c_str());

    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(6.9), 80);

    hal->gfx()->print(date_Array);
    hal->gfx()->print(" ");
    hal->gfx()->printDecimal(monthInt, 2);
    hal->gfx()->print("/");
    hal->gfx()->printDecimal(dayInt, 2);
    hal->gfx()->print("/");
    hal->gfx()->print(yearInt);
}

void printTime(OswHal* hal, uint32_t hour, uint32_t minute, uint32_t second) {
  hal->gfx()->printDecimal(hour, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(minute, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(second, 2);
}

void drawTime(OswHal* hal) {
  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;
  bool afterNoon = false;
  char am[] = "AM";
  char pm[] = "PM";

  hal->gfx()->setTextSize(3);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(5.5), 120);

  hal->getLocalTime(&hour, &minute, &second, &afterNoon);
  printTime(hal, hour, minute, second);
  hal->gfx()->print(" ");
  if (afterNoon){
      hal->gfx()->print(pm);
  } else {
      hal->gfx()->print(am);
  }

}

void drawTime24Hour(OswHal* hal) {
  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;

  hal->gfx()->setTextSize(4);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(4), 120);

  hal->getLocalTime(&hour, &minute, &second);
  printTime(hal, hour, minute, second);
}

void drawSteps(OswHal* hal) {
  uint32_t steps = hal->getStepCount();
  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextCursor(((220/2)-defaultFontXOffset(1, 2)), 210 - defaultFontYOffset(1, 2) / 2);

  hal->gfx()->print(steps);
}

void OswAppWatchfaceDigital::setup(OswHal* hal) {

}

void OswAppWatchfaceDigital::loop(OswHal* hal) {
  if(hal->btn2Down()) {
    hal->decreaseBrightness(25);
    hal->clearBtn2();
  }
  if (hal->btn3Down()) {
    hal->increaseBrightness(25);
    hal->clearBtn3();
  }

  hal->gfx()->fill(COLOR_BLACK);

  drawDate(hal);

  if (TIME_FORMAT == 12) {
    drawTime(hal);
  } else {
    drawTime24Hour(hal);
  }

  drawSteps(hal);

  hal->requestFlush();
}

void OswAppWatchfaceDigital::stop(OswHal* hal) {
  // hal->disableDisplayBuffer();
}