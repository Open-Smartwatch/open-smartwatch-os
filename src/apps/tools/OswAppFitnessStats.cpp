#include "./apps/tools/OswAppFitnessStats.h"
#include "./apps/watchfaces/OswAppWatchfaceDual.h"
#include "./apps/watchfaces/OswAppWatchfaceFitness.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_pins.h>

void OswAppFitnessStats::showFitnessTracking() {
  OswHal* hal = OswHal::getInstance();

  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextCenterAligned();

  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextCursor(DISP_H / 2, 60);
  hal->gfx()->setTextColor(ui->getForegroundColor());
  hal->gfx()->print(LANG_FITNESS_TITLE);

  const char* type[] = {LANG_FITNESS_STEP, "kcal", LANG_FITNESS_DISTANCE, LANG_FITNESS_TOTAL};
  int fitnessGoal[] = {OswConfigAllKeys::stepsPerDay.get(), OswConfigAllKeys::distPerDay.get(),OswConfigAllKeys::kcalPerDay.get(),0};
  fitnessGoal[3]=(fitnessGoal[0]+fitnessGoal[1]+fitnessGoal[2])/3;
  uint32_t fitnessValue[] = {hal->environment->getStepsToday(),OswAppWatchfaceFitness::calculateDistance(hal->environment->getStepsToday()),OswAppWatchfaceFitness::calculateKcalorie(hal->environment->getStepsToday()),0};
  fitnessValue[3] = (fitnessValue[0] + fitnessValue[1]+fitnessValue[2])/3;

  uint16_t fitnesColor[] = {ui->getInfoColor(),ui->getWarningColor(),ui->getDangerColor(),ui->getPrimaryColor()};
  uint8_t x_point = 50;
  uint8_t level_y = 30;
  uint8_t start_y = 90;

   for (uint8_t i = 0; i < 4; i++) {
    uint32_t s = fitnessValue[i];
    uint16_t thickTickWidth = ((float)(s > fitnessGoal[i] ? fitnessGoal[i] : s) / fitnessGoal[i]) * ((DISP_H / 2 - x_point) + (DISP_H / 2) - x_point);
    thickTickWidth = thickTickWidth < 2 ? 0 : thickTickWidth;
    OswAppWatchfaceDual::drawProgressBar(ui,x_point, start_y + i * level_y, 0, (DISP_H / 2 - x_point) + (DISP_H / 2) - x_point, thickTickWidth, 90, 10, fitnesColor[i], &fitnessGoal[i]);
    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextRightAligned();

    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextCursor(240 - x_point, start_y + i * level_y);
    hal->gfx()->setTextColor(ui->getForegroundColor());

    hal->gfx()->print(fitnessValue[i]+ String("/") + fitnessGoal[i]);
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextCursor(x_point, start_y + i * level_y);
    hal->gfx()->print(type[i]);
  }
}

void OswAppFitnessStats::setup() {

}
void OswAppFitnessStats::loop() {
  OswHal* hal = OswHal::getInstance();

  hal->gfx()->fill(ui->getBackgroundColor());
  showFitnessTracking();
  hal->requestFlush();
}

void OswAppFitnessStats::stop() {
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
  OswHal::getInstance()->gpsBackupMode();
#endif
}
