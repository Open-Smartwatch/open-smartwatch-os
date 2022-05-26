#ifdef OSW_FEATURE_STATS_STEPS

#include "./apps/tools/OswAppStepStats.h"
#include "./apps/watchfaces/OswAppWatchfaceFitness.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_pins.h>

void OswAppStepStats::drawChart() {
  OswHal* hal = OswHal::getInstance();
  uint8_t chartStickHeight = 55;

  uint8_t interval = 20;
  uint16_t goalValue = OswConfigAllKeys::stepsPerDay.get();

  uint32_t weekDay = 0;
  uint32_t dayOfMonth = 0;
  hal->getLocalDate(&dayOfMonth, &weekDay);

  for (uint8_t index = 0; index < 7; index++) {
    uint32_t weekDayStep = hal->environment->getStepsOnDay(index);  // virtual step simulation
    uint16_t chartStickValue = ((float)(weekDayStep > goalValue ? goalValue : weekDayStep) / goalValue) * chartStickHeight;

    uint16_t barColor = OswConfigAllKeys::stepsPerDay.get() <= weekDayStep ? ui->getSuccessColor() : changeColor(ui->getSuccessColor(),2.85);

    chartStickValue = chartStickValue < 2 ? 0 : chartStickValue;
    // step bars

    if (index == weekDay) {
      hal->gfx()->drawThickTick(60 + index * interval, 150 - 3, 0, chartStickHeight, 0, 5, ui->getForegroundColor());
    }
    hal->gfx()->drawThickTick(60 + index * interval, 150 - 3, 0, chartStickValue, 0, 3, barColor, true);
  }
}

void OswAppStepStats::showStickChart() {
  OswHal* hal = OswHal::getInstance();

  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextCenterAligned();

  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextCursor(DISP_H / 2, 60);
  hal->gfx()->setTextColor(ui->getForegroundColor());
  hal->gfx()->print(LANG_STEPSTATS_TITLE);

  OswAppStepStats::drawChart();

  uint8_t coord_x = 30;

  hal->gfx()->drawThickTick(coord_x, 150, 0, 240 - (coord_x * 2), 90, 2, ui->getPrimaryColor());
  hal->gfx()->drawLine(120, 150 + 15, 120, 220, ui->getPrimaryColor());  // long front

  hal->gfx()->setTextSize(1);
  hal->gfx()->setTextCenterAligned();
  hal->gfx()->setTextBottomAligned();
  hal->gfx()->setTextCursor(80, 140 + 25);
  hal->gfx()->print(LANG_STEPSTATS_TOTAL);  // total step counter
  hal->gfx()->setTextCursor(160, 140 + 25);
  hal->gfx()->print(LANG_STEPSTATS_TODAY);  // total step counter

  hal->gfx()->setTextRightAligned();
  hal->gfx()->setTextCursor(120 - 7, 160 + 25);
  hal->gfx()->print(OswAppWatchfaceFitness::calculateDistance(hal->environment->getStepsTotal())  );
  hal->gfx()->setTextCursor(120 - 7, 160 + 25 + 10);
  hal->gfx()->print(hal->environment->getStepsTotal()  );  // total step counter
  hal->gfx()->setTextCursor(120 - 7, 160 + 25 + 20);
  hal->gfx()->print(OswAppWatchfaceFitness::calculateKcalorie(hal->environment->getStepsTotal()) );  // total step counter
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(120 + 7, 160 + 25);
  hal->gfx()->print(OswAppWatchfaceFitness::calculateDistance(hal->environment->getStepsToday())+String(" m"));
  hal->gfx()->setTextCursor(120 + 7, 160 + 25 + 10);
  hal->gfx()->print(hal->environment->getStepsToday() +String(" ")+ String(LANG_STEPSTATS_STEP));  // total step counter
  hal->gfx()->setTextCursor(120 + 7, 160 + 25 + 20);
  hal->gfx()->print(OswAppWatchfaceFitness::calculateKcalorie(hal->environment->getStepsToday())+String(" kcal"));  // total step counter
}

void OswAppStepStats::setup() {}
void OswAppStepStats::loop() {
  OswHal* hal = OswHal::getInstance();
  hal->gfx()->fill(ui->getBackgroundColor());
  showStickChart();
  hal->requestFlush();
}

void OswAppStepStats::stop() {}
#endif