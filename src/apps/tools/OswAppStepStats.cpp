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
        uint32_t weekDayStep = hal->environment->getStepsOnDay(index);
        uint16_t chartStickValue = ((float)(weekDayStep > goalValue ? goalValue : weekDayStep) / goalValue) * chartStickHeight;

        uint16_t barColor = OswConfigAllKeys::stepsPerDay.get() <= weekDayStep ? ui->getSuccessColor() : changeColor(ui->getSuccessColor(),2.85);

        chartStickValue = chartStickValue < 2 ? 0 : chartStickValue;

        if (index == weekDay) {
            hal->gfx()->drawThickTick(60 + index * interval, 147, 0, chartStickHeight, 0, 5, ui->getForegroundColor());
        }
        hal->gfx()->drawThickTick(60 + index * interval, 147, 0, chartStickValue, 0, 3, barColor, true);
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
    hal->gfx()->drawLine(120, 165, 120, 220, ui->getPrimaryColor());  // long front

    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextBottomAligned();
    hal->gfx()->setTextCursor(80, 165);
    hal->gfx()->print(LANG_STEPSTATS_TOTAL);
    hal->gfx()->setTextCursor(160, 165);
    hal->gfx()->print(LANG_STEPSTATS_TODAY);

    hal->gfx()->setTextRightAligned();
    hal->gfx()->setTextCursor(DISP_W / 2 - 7, 185);
    hal->gfx()->print(String(OswAppWatchfaceFitness::calculateDistance(hal->environment->getStepsTotalWeek())));
    hal->gfx()->setTextCursor(DISP_W / 2 - 7, 195);
    hal->gfx()->print(hal->environment->getStepsTotalWeek()  );
    hal->gfx()->setTextCursor(DISP_W / 2 - 7, 205);
    hal->gfx()->print(OswAppWatchfaceFitness::calculateKcalorie(hal->environment->getStepsTotalWeek()) );
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextCursor(DISP_W / 2 + 7, 185);
    hal->gfx()->print(OswAppWatchfaceFitness::calculateDistance(hal->environment->getStepsToday())+String(" m"));
    hal->gfx()->setTextCursor(DISP_W / 2 + 7, 195);
    hal->gfx()->print(hal->environment->getStepsToday() +String(" ")+ String(LANG_STEPSTATS_STEP));
    hal->gfx()->setTextCursor(DISP_W / 2  + 7, 205);
    hal->gfx()->print(OswAppWatchfaceFitness::calculateKcalorie(hal->environment->getStepsToday())+String(" kcal"));
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