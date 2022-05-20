#include "./apps/watchfaces/OswAppWatchfaceFitness.h"
#include "./apps/watchfaces/OswAppWatchfaceDigital.h"

#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <time.h>

#include "bma400_defs.h"

uint32_t OswAppWatchfaceFitness::calculateDistance(uint32_t steps) {
  uint8_t user_height = OswConfigAllKeys::configHeight.get();
  uint16_t avg_dis = ((user_height * 0.37) + (user_height * 0.45) + (user_height - 100)) / 3;
  return steps * avg_dis * 0.01 * 0.001;  // cm -> m -> km
}

uint32_t OswAppWatchfaceFitness::calculateKcalorie(uint32_t steps) {
  uint8_t kcal_steps = 30;    // Generally, 1 kcal is consumed at 30 steps.
  return steps / kcal_steps;  // There is an error in this. By giving a percentage, people can see more exercise
                              // effects by marking fewer calories than they actually consumed.
}

void dateDisplay(const uint8_t& showDateFormat) {
  uint32_t dayInt = 0;
  uint32_t monthInt = 0;
  uint32_t yearInt = 0;
  OswHal* hal = OswHal::getInstance();
  const char* weekday = hal->getLocalWeekday();

  hal->getLocalDate(&dayInt, &monthInt, &yearInt);

  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextRightAligned();
  hal->gfx()->setTextCursor(205, 90);

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
  // hal->gfx()->print(", ");

  // Date
  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextRightAligned();
  hal->gfx()->setTextCursor(205, 150);
  OswAppWatchfaceDigital::dateOutput(yearInt, monthInt, dayInt);
}

void timeDisplay(uint32_t hour, uint32_t minute, uint32_t second) {
  OswHal* hal = OswHal::getInstance();
  hal->gfx()->printDecimal(hour, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(minute, 2);

  hal->gfx()->setTextSize(1);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(215, 120);
  hal->gfx()->printDecimal(second,2);
}

void digitalWatchDisplay() {
  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;
  bool afterNoon = false;
  char am[] = "AM";
  char pm[] = "PM";
  OswHal* hal = OswHal::getInstance();

  hal->gfx()->setTextSize(4);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(120 - 30, 120);

  hal->getLocalTime(&hour, &minute, &second, &afterNoon);

  timeDisplay(hour, minute, second);
  if (!OswConfigAllKeys::timeFormat.get()) {
    hal->gfx()->setTextCursor(215, 130);
    if (afterNoon) {
      hal->gfx()->print(pm);
    } else {
      hal->gfx()->print(am);
    }
  }
}
void OswAppWatchfaceFitness::showFitnessTracking() {
  OswHal* hal = OswHal::getInstance();  
  uint32_t steps = hal->environment->getStepsToday();
  uint32_t dists = OswAppWatchfaceFitness::calculateDistance(steps);
  uint32_t kcals = OswAppWatchfaceFitness::calculateKcalorie(steps);
  uint32_t stepsTarget = OswConfigAllKeys::stepsPerDay.get();
  uint32_t distTarget = OswConfigAllKeys::distPerDay.get();
  uint32_t kcalTarget = OswConfigAllKeys::kcalPerDay.get();

  hal->gfx()->drawArc(DISP_W/2, DISP_H/2, 180, 360, 90, 93, 7, changeColor(ui->getDangerColor(), 0.25));
  // hal->gfx()->drawArc(DISP_W/2, DISP_H/2, 180, 280, 90, 93, 7, dimColor(rgb565(210, 50, 66), 25));
  hal->gfx()->drawArc(DISP_W/2, DISP_H/2, 180, 180 + (180.0 * (float)(steps % stepsTarget) / (float)stepsTarget), 90, 93, 6, steps > stepsTarget ? changeColor(ui->getSuccessColor(),6.25 ): ui->getDangerColor(), true);

  hal->gfx()->drawArc(DISP_W/2, DISP_H/2, 180, 360, 90, 75, 7, changeColor(ui->getWarningColor(), 0.25));
  // hal->gfx()->drawArc(DISP_W/2, DISP_H/2, 180, 180+128, 90, 75, 7, dimColor(rgb565(117, 235, 10), 25));
  hal->gfx()->drawArc(DISP_W/2, DISP_H/2, 180, 180 + (180.0 * (float)(dists % distTarget) / (float)distTarget), 90, 75, 6, dists > distTarget ? changeColor(ui->getSuccessColor() ,1.25): ui->getWarningColor(), true);

  hal->gfx()->drawArc(DISP_W/2, DISP_H/2, 180, 360, 90, 57, 7, changeColor(ui->getInfoColor(), 0.25));
  // hal->gfx()->drawArc(DISP_W/2, DISP_H/2, 180, 180+32, 90, 57, 7, dimColor(rgb565(25, 193, 202), 25));
  hal->gfx()->drawArc(DISP_W/2, DISP_H/2, 180, 180 + (180.0 * (float)(kcals % kcalTarget) / (float)kcalTarget), 90, 57, 6, kcals > kcalTarget ? changeColor(ui->getSuccessColor(),2.25) : ui->getInfoColor(), true);

  hal->gfx()->setTextSize(1);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextLeftAligned();

  hal->gfx()->setTextColor(dimColor(ui->getDangerColor(), 25));
  hal->gfx()->setTextCursor(120 + 10, 25);
  hal->gfx()->print(steps);

  hal->gfx()->setTextColor(dimColor(ui->getWarningColor(), 25));
  hal->gfx()->setTextCursor(120 + 10, 45);
  hal->gfx()->print(kcals);

  hal->gfx()->setTextColor(dimColor(ui->getInfoColor(), 25));
  hal->gfx()->setTextCursor(120 + 10, 65);
  hal->gfx()->print(dists);

  hal->gfx()->setTextColor(dimColor(ui->getInfoColor(), 25));
  hal->gfx()->setTextCursor(120 + 10, 180 - 5);
  hal->gfx()->print("Dist");

  hal->gfx()->setTextColor(dimColor(ui->getWarningColor(), 25));
  hal->gfx()->setTextCursor(120 + 10, 200 - 5);
  hal->gfx()->print("Kcal");

  hal->gfx()->setTextColor(dimColor(ui->getDangerColor(), 25));
  hal->gfx()->setTextCursor(120 + 10, 220 - 5);
  hal->gfx()->print("Step");
}
void OswAppWatchfaceFitness::setup() { 

}

void OswAppWatchfaceFitness::loop() {
  OswHal* hal = OswHal::getInstance();
  if (hal->btnHasGoneDown(BUTTON_3)) {
    hal->increaseBrightness(25);
  }
  if (hal->btnHasGoneDown(BUTTON_2)) {
    hal->decreaseBrightness(25);
  }

  hal->gfx()->fill(ui->getBackgroundColor());

  dateDisplay(OswAppWatchfaceDigital::getDateFormat());

  digitalWatchDisplay();

  #if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
  showFitnessTracking();
  #endif

  hal->requestFlush();
}

void OswAppWatchfaceFitness::stop() {
  // OswHal::getInstance()->disableDisplayBuffer();
}
