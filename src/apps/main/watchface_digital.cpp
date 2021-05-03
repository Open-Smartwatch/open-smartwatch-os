
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
    string day = " ";
    uint32_t dayInt = 0;
    uint32_t monthInt = 0;
    uint32_t yearInt = 0;
    int charLen = 3;
    hal->getWeekdayString(charLen, &day);
    hal->getDate(&dayInt, &monthInt, &yearInt);

    //we want to output a value like "Wed, 05/02/2021"
    char date_Array[charLen + 1];

    strcpy(date_Array, day.c_str());

    hal->getCanvas()->setTextSize(2);
    hal->getCanvas()->setCursor(((220/2)-defaultFontXOffset(6, 2)), 80 - defaultFontYOffset(1, 2) / 2);

    hal->getCanvas()->print(date_Array);
    hal->getCanvas()->print(" ");
    print2Digits(hal, monthInt);
    hal->getCanvas()->print("/");
    print2Digits(hal, dayInt);
    hal->getCanvas()->print("/");
    hal->getCanvas()->print(yearInt);
}

void printTime(OswHal* hal, uint32_t hour, uint32_t minute, uint32_t second) {
  print2Digits(hal, hour);
  hal->getCanvas()->print(":");
  print2Digits(hal, minute);
  hal->getCanvas()->print(":");
  print2Digits(hal, second);
}

void drawTime(OswHal* hal) {
  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;
  bool afterNoon = false;
  char am[] = "AM";
  char pm[] = "PM";

  hal->getCanvas()->setTextSize(3);
  hal->getCanvas()->setCursor(defaultFontXOffset(1, 4), 130 - defaultFontYOffset(1, 4) / 2);

  hal->getLocalTime12h(&hour, &minute, &second, &afterNoon);
  printTime(hal, hour, minute, second);
  hal->getCanvas()->print(" ");
  if (afterNoon){
      hal->getCanvas()->print(pm);
  } else {
      hal->getCanvas()->print(am);
  }
  
}

void drawTime24Hour(OswHal* hal) {
  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;

  hal->getCanvas()->setTextSize(4);
  hal->getCanvas()->setCursor(defaultFontXOffset(1, 4), 120 - defaultFontYOffset(1, 4) / 2);

  hal->getLocalTime(&hour, &minute, &second);
  printTime(hal, hour, minute, second);
}

void drawSteps(OswHal* hal) {
  uint32_t steps = hal->getStepCount();
  hal->getCanvas()->setTextSize(2);
  hal->getCanvas()->setCursor(((220/2)-defaultFontXOffset(1, 2)), 200 - defaultFontYOffset(1, 2) / 2);

  hal->getCanvas()->print(steps);
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

  hal->requestFlush();
  hal->getCanvas()->getGraphics2D()->fill(COLOR_BLACK);

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
