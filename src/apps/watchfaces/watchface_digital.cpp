#include "./apps/watchfaces/watchface_digital.h"
#include "./apps/watchfaces/watchface.h"

#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <time.h>

#define COLOR_BLACK rgb565(0, 0, 0)

void drawDate(const uint8_t& showDateFormat) {
  uint32_t dayInt = 0;
  uint32_t monthInt = 0;
  uint32_t yearInt = 0;
  OswHal* hal = OswHal::getInstance();
  const char* weekday = hal->getWeekday();

  hal->getDate(&dayInt, &monthInt, &yearInt);

  // we want to output a value like "Wed, 05/02/2021"

  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(6.9), 80);

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

  // i really would want the date to be dynamic based on what's in the config, but the most efficient thing to do right
  // now is simply three if statements covering the 3 common conditions.
  switch(showDateFormat) {
    case 1:  // 0 : mm/dd/yyyy
      hal->gfx()->printDecimal(monthInt, 2);
      hal->gfx()->print("/");
      hal->gfx()->printDecimal(dayInt, 2);
      hal->gfx()->print("/");
      hal->gfx()->print(yearInt);
      break;
    case 2:  // 1 : dd.mm.yyyy
      hal->gfx()->printDecimal(dayInt, 2);
      hal->gfx()->print(".");
      hal->gfx()->printDecimal(monthInt, 2);
      hal->gfx()->print(".");
      hal->gfx()->print(yearInt);
      break;
    case 3:  // 2 : yy.mm/dd
      hal->gfx()->print(yearInt);
      hal->gfx()->print(".");
      hal->gfx()->printDecimal(monthInt, 2);
      hal->gfx()->print("/");
      hal->gfx()->printDecimal(dayInt, 2);
      break;
  }
}

void timeOutput(uint32_t hour, uint32_t minute, uint32_t second) {
  OswHal* hal = OswHal::getInstance();
  hal->gfx()->printDecimal(hour, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(minute, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(second, 2);
}

void drawTime() {
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
  hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(5.5), 120);

  hal->getLocalTime(&hour, &minute, &second, &afterNoon);
  timeOutput(hour, minute, second);
  hal->gfx()->print(" ");
  if (afterNoon) {
    hal->gfx()->print(pm);
  } else {
    hal->gfx()->print(am);
  }
}

void drawTime24Hour() {
  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;
  OswHal* hal = OswHal::getInstance();

  hal->gfx()->setTextSize(4);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(4), 120);

  hal->getLocalTime(&hour, &minute, &second);
  timeOutput(hour, minute, second);
}

void drawSteps() {
#ifdef OSW_FEATURE_STATS_STEPS
  uint8_t w = 8;
  OswAppWatchface::drawStepHistory(OswUI::getInstance(), (DISP_W / 2) - w * 3.5, 180, w, w * 4, OswConfigAllKeys::stepsPerDay.get());
#else
  OswHal* hal = OswHal::getInstance();
  uint32_t steps = hal->environment->getStepsToday();
  hal->gfx()->setTextCenterAligned();
  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextCursor(120, 210 - hal->gfx()->getTextOfsetRows(1) / 2);

  hal->gfx()->print(steps);
#endif
}

void OswAppWatchfaceDigital::setup() { 
  showDateFormat = (OswConfigAllKeys::dateFormat.get() == "mm/dd/yyyy" ? 1 : 
                   (OswConfigAllKeys::dateFormat.get() == "dd.mm.yyyy" ? 2 : 3 ) ); 
}

void OswAppWatchfaceDigital::loop() {
  OswHal* hal = OswHal::getInstance();
  if (hal->btnHasGoneDown(BUTTON_3)) {
    hal->increaseBrightness(25);
  }
  if (hal->btnHasGoneDown(BUTTON_2)) {
    hal->decreaseBrightness(25);
  }

  hal->gfx()->fill(ui->getBackgroundColor());

  drawDate(this->showDateFormat);

  if (!OswConfigAllKeys::timeFormat.get()) {
    drawTime();
  } else {
    drawTime24Hour();
  }

  #if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
  drawSteps();
  #endif

  hal->requestFlush();
}

void OswAppWatchfaceDigital::stop() {
  // OswHal::getInstance()->disableDisplayBuffer();
}
