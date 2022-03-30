// test - header file

#include "./apps/watchfaces/watchface_digital.h"
// test
#include "./apps/watchfaces/watchface_mix.h"
#include "./apps/watchfaces/watchface.h"

#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <time.h>

#include "bma400_defs.h"

#define COLOR_BLACK rgb565(0, 0, 0)


void stepsDisplay() {
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

void analogWatchDisplay() {
  OswHal* hal = OswHal::getInstance();
  uint32_t second = 0;
  uint32_t minute = 0;  // Unused, but required by function signature
  uint32_t hour = 0;    // Unused, but required by function signature

  hal->getLocalTime(&hour, &minute, &second);
  hal->gfx()->drawCircle((int)(DISP_W * 0.5) - 55, 100, 50, rgb565(255, 255, 255));
  hal->gfx()->drawHourTicks((int)(DISP_W*0.5)-55, 100, 45, 40, rgb565(255, 255, 255));

  // hour
  hal->gfx()->drawLine((int)(DISP_W*0.5)-55, 100, rpx((int)(DISP_W*0.5)-55, (int)(33 * 0.5), hour * 30  + (int)( minute* 0.1 ) * 6), rpy(100, (int)(33 * 0.5), hour * 30 + (int)( minute* 0.1 ) * 6 ), rgb565(255, 255, 255)); 
  // // minute
  hal->gfx()->drawLine((int)(DISP_W*0.5)-55, 100, rpx((int)(DISP_W*0.5)-55, (int)(66 * 0.5),  minute * 6), rpy(100, (int)(66 * 0.5),  minute * 6), rgb565(0, 255, 0)); 
  // // second
  hal->gfx()->drawLine((int)(DISP_W*0.5)-55, 100, rpx((int)(DISP_W*0.5)-55, (int)(15 * 0.5), s2d(second) + 180), rpy(100, (int)(15 * 0.5), s2d(second) + 180), rgb565(255, 0, 0));  // short backwards
  hal->gfx()->drawLine((int)(DISP_W*0.5)-55, 100, rpx((int)(DISP_W*0.5)-55, (int)(90 * 0.5), s2d(second)), rpy(100, (int)(90 * 0.5), s2d(second)), rgb565(255, 0, 0));  // long front
}

void dateDisplay(const uint8_t& showDateFormat) {
  uint32_t dayInt = 0;
  uint32_t monthInt = 0;
  uint32_t yearInt = 0;
  OswHal* hal = OswHal::getInstance();
  const char* weekday = hal->getWeekday();

  hal->getDate(&dayInt, &monthInt, &yearInt);

  // we want to output a value like "Wed, 05/02/2021"

  hal->gfx()->setTextSize(1);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(123, 75);

  {
    char weekday3[4];
    weekday3[0] = weekday[0];
    weekday3[1] = weekday[1];
    weekday3[2] = weekday[2];
    weekday3[3] = '\0';
    hal->gfx()->print(weekday3);
  }

  // The GFX library has an alignment bug, causing single letters to "float", therefore the workaround above is used to still utilize the correct string printing.
  //hal->gfx()->print(weekday[0]);
  //hal->gfx()->print(weekday[1]);
  //hal->gfx()->print(weekday[2]);
  hal->gfx()->print(", ");

  // Date
  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(123, 90);

  // i really would want the date to be dynamic based on what's in the config, but the most efficient thing to do right
  // now is simply three if statements covering the 3 common conditions.
  switch (showDateFormat) {
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
      hal->gfx()->printDecimal(dayInt, 2);
      break;
  }
}

void timeDisplay(uint32_t hour, uint32_t minute, uint32_t second) {
  OswHal* hal = OswHal::getInstance();
  hal->gfx()->printDecimal(hour, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(minute, 2);
}

void digitalWatchDisplay() {
  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;
  bool afterNoon = false;
  char am[] = "AM";
  char pm[] = "PM";
  OswHal* hal = OswHal::getInstance();

  hal->gfx()->setTextSize(3);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(123, 120);

  hal->getLocalTime(&hour, &minute, &second, &afterNoon);
  timeDisplay(hour, minute, second);
  if (!OswConfigAllKeys::timeFormat.get()) {
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextBottomAligned();
    hal->gfx()->setTextCursor(120 + 100, 120 + 10);

    hal->gfx()->print(" ");
    if (afterNoon) {
      hal->gfx()->print(pm);
    } else {
      hal->gfx()->print(am);
    }
  }
}

void OswAppWatchfaceMix::setup() { 
  showDateFormat = (OswConfigAllKeys::dateFormat.get() == "mm/dd/yyyy" ? 1 : 
                (OswConfigAllKeys::dateFormat.get() == "dd.mm.yyyy" ? 2 : 3 ) ); 
}

void OswAppWatchfaceMix::loop() {
  OswHal* hal = OswHal::getInstance();
  if (hal->btnHasGoneDown(BUTTON_3)) {
    hal->increaseBrightness(25);
  }
  if (hal->btnHasGoneDown(BUTTON_2)) {
    hal->decreaseBrightness(25);
  }

  hal->gfx()->fill(ui->getBackgroundColor());

  analogWatchDisplay();

  dateDisplay(this->showDateFormat);

  digitalWatchDisplay();

  #if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
  stepsDisplay();
  #endif

  hal->requestFlush();
}

void OswAppWatchfaceMix::stop() {
  // OswHal::getInstance()->disableDisplayBuffer();
}
