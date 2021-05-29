#include "./apps/watchfaces/watchface_digital.h"

#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <time.h>

#include <string>
using std::string;

#define COLOR_BLACK rgb565(0, 0, 0)

void drawDate(OswHal* hal, const bool& useMMDDYYYY) {
  string day = "";
  uint32_t dayInt = 0;
  uint32_t monthInt = 0;
  uint32_t yearInt = 0;
  int charLen = 3;
  hal->getWeekdayString(charLen, &day);
  hal->getDate(&dayInt, &monthInt, &yearInt);

  // we want to output a value like "Wed, 05/02/2021"
  char date_Array[charLen + 1];

  strcpy(date_Array, day.c_str());

  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(6.9), 80);

  hal->gfx()->print(date_Array);
  hal->gfx()->print(", ");

  // i really would want the date to be dynamic based on what's in the config, but the most efficient thing to do right
  // now is simply two if statements covering the 2 common conditions.
  if (useMMDDYYYY) {
    hal->gfx()->printDecimal(monthInt, 2);
    hal->gfx()->print("/");
    hal->gfx()->printDecimal(dayInt, 2);
    hal->gfx()->print("/");
  } else {
    hal->gfx()->printDecimal(dayInt, 2);
    hal->gfx()->print(".");
    hal->gfx()->printDecimal(monthInt, 2);
    hal->gfx()->print(".");
  }

  hal->gfx()->print(yearInt);
}

void timeOutput(OswHal* hal, uint32_t hour, uint32_t minute, uint32_t second) {
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
  timeOutput(hal, hour, minute, second);
  hal->gfx()->print(" ");
  if (afterNoon) {
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
  timeOutput(hal, hour, minute, second);
}

void drawSteps(OswHal* hal) {
  uint32_t steps = hal->getStepCount();
  hal->gfx()->setTextCenterAligned();
  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextCursor(120, 210 - hal->gfx()->getTextOfsetRows(1) / 2);

  hal->gfx()->print(steps);
}

void OswAppWatchfaceDigital::setup(OswHal* hal) {
  useMMDDYYYY = OswConfigAllKeys::dateFormat.get() == "mm/dd/yyyy";
}

void OswAppWatchfaceDigital::loop(OswHal* hal) {
  if (hal->btnHasGoneDown(BUTTON_3)) {
    hal->increaseBrightness(25);
  }
  if (hal->btnHasGoneDown(BUTTON_2)) {
    hal->decreaseBrightness(25);
  }

  hal->gfx()->fill(ui->getBackgroundColor());

  drawDate(hal, this->useMMDDYYYY);

  if (!OswConfigAllKeys::timeFormat.get()) {
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
