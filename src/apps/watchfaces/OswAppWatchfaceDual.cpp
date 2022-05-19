#include "./apps/watchfaces/OswAppWatchfaceDual.h"

#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <time.h>

#include "./apps/watchfaces/OswAppWatchface.h"
#include "./apps/watchfaces/OswAppWatchfaceDigital.h"
#include "bma400_defs.h"

void displayStep() {
#ifdef OSW_FEATURE_STATS_STEPS
  uint8_t w = 8;
  OswAppWatchface::drawStepHistory(OswUI::getInstance(), (DISP_W / 2) - w * 3.5, 180, w, w * 4,
                                   OswConfigAllKeys::stepsPerDay.get());
#else
  OswHal* hal = OswHal::getInstance();
  uint32_t steps = hal->environment->getStepsToday();
  hal->gfx()->setTextCenterAligned();
  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextCursor(120, 210 - hal->gfx()->getTextOfsetRows(1) / 2);

  hal->gfx()->print(steps);
#endif
}

void OswAppWatchfaceDual::digitalDate(uint8_t dateCoordX, short timeZone) {
  uint32_t dayInt = 0;
  uint32_t monthInt = 0;
  uint32_t yearInt = 0;
  OswHal* hal = OswHal::getInstance();
  const char* weekday = hal->getWeekday(timeZone);

  hal->getDate(timeZone,&dayInt, &monthInt, &yearInt);

  // we want to output a value like "Wed, 05/02/2021"

  hal->gfx()->setTextSize(1);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(dateCoordX, 75);

  {
    char weekday3[4];
    weekday3[0] = weekday[0];
    weekday3[1] = weekday[1];
    weekday3[2] = weekday[2];
    weekday3[3] = '\0';
    hal->gfx()->print(weekday3);
  }

  // The GFX library has an alignment bug, causing single letters to "float", therefore the workaround above is used to
  // still utilize the correct string printing.
  // hal->gfx()->print(weekday[0]);
  // hal->gfx()->print(weekday[1]);
  // hal->gfx()->print(weekday[2]);

  ////date
  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextBottomAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(dateCoordX, 98);

  // i really would want the date to be dynamic based on what's in the config, but the most efficient thing to do
  // right now is simply three if statements covering the 3 common conditions.
  switch (OswAppWatchfaceDigital::getDateFormat()) {
    case 1:  // 0 : mm/dd/yyyy
      hal->gfx()->printDecimal(monthInt, 2);
      hal->gfx()->print("/");
      hal->gfx()->printDecimal(dayInt, 2);
      hal->gfx()->print("/");
      hal->gfx()->printDecimal(yearInt % 100, 2);
      break;
    case 2:  // 1 : dd.mm.yyyy
      hal->gfx()->printDecimal(dayInt, 2);
      hal->gfx()->print(".");
      hal->gfx()->printDecimal(monthInt, 2);
      hal->gfx()->print(".");
      hal->gfx()->printDecimal(yearInt % 100, 2);
      break;
    case 3:  // 2 : yy.mm/dd
      hal->gfx()->printDecimal(yearInt % 100, 2);
      hal->gfx()->print(".");
      hal->gfx()->printDecimal(monthInt, 2);
      hal->gfx()->print("/");
      hal->gfx()->printDecimal(dayInt,2);
      break;
  }
}

void timeDisplay(uint32_t hour, uint32_t minute, uint32_t second, bool showSecond) {
  OswHal* hal = OswHal::getInstance();
  hal->gfx()->printDecimal(hour, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(minute, 2);
  if(showSecond){
    hal->gfx()->print(":");
    hal->gfx()->printDecimal(second,2);
  }
}

void OswAppWatchfaceDual::digitalTime(short timeZone, bool showSecond, uint8_t timeCoord, uint8_t AMPMCoord) {
  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;
  bool afterNoon = false;
  char am[] = "AM";
  char pm[] = "PM";
  OswHal* hal = OswHal::getInstance();
 
  //time
  hal->gfx()->setTextSize(3);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextBottomAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(timeCoord, 130);

  hal->getTime(timeZone,&hour, &minute, &second);
  timeDisplay(hour, minute, second, showSecond);
  if (!OswConfigAllKeys::timeFormat.get()) {
    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextBottomAligned();
    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextSize(3);
    hal->gfx()->setTextCursor(timeCoord+AMPMCoord, 150);  // right
    hal->gfx()->setTextSize(1);

    if (afterNoon) {
      hal->gfx()->print(pm);
    } else {
      hal->gfx()->print(am);
    }
  }
}

void commonSecondsDisplay() {
  OswHal* hal = OswHal::getInstance();
  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;
  hal->getLocalTime(&hour, &minute, &second);

  hal->gfx()->setTextSize(1);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextCenterAligned();
  hal->gfx()->setTextCursor(DISP_W/2, 150);
  hal->gfx()->print(second);
}

void OswAppWatchfaceDual::digitalClock(short timeZone, const char str[], uint8_t titleCoordX,bool showSecond,uint8_t commonCoord,uint8_t AMPMCoord) {
  OswHal* hal = OswHal::getInstance();

  hal->gfx()->setTextSize(1);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextBottomAligned();
  hal->gfx()->setTextCursor(titleCoordX, 65);
  hal->gfx()->print(str);

  digitalDate(timeZone, commonCoord);
  digitalTime(timeZone, showSecond, commonCoord, AMPMCoord);

}

void OswAppWatchfaceDual::setup() {}

void OswAppWatchfaceDual::loop() {
  OswHal* hal = OswHal::getInstance();
  if (hal->btnHasGoneDown(BUTTON_3)) {
    hal->increaseBrightness(25);
  }
  if (hal->btnHasGoneDown(BUTTON_2)) {
    hal->decreaseBrightness(25);
  }

  hal->gfx()->fill(ui->getBackgroundColor());

  // Right - Local Time
  digitalClock(OswConfigAllKeys::timeZone.get(), "Local - time", DISP_W / 2 + startCoord + 5, false,
               DISP_W / 2 + startCoord, hal->gfx()->getTextOfsetColumns(5.25));

  // Left - Dual Time
  digitalClock(OswConfigAllKeys::dualTimeZone.get(), "Dual - time", startCoord + 5, false,  startCoord,
               hal->gfx()->getTextOfsetColumns(5.25));

  commonSecondsDisplay();

#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
  displayStep();
#endif

  hal->requestFlush();
}

void OswAppWatchfaceDual::stop() {
  // OswHal::getInstance()->disableDisplayBuffer();
}
