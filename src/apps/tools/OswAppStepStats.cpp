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

        if (index == cursorPos) {
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

    hal->gfx()->drawThickTick(coord_x, 150, 0, DISP_W - (coord_x * 2), 90, 2, ui->getPrimaryColor());
    uint32_t tmpCursor = cursorPos;
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextBottomAligned();
    hal->gfx()->setTextCursor(120, 170);
    hal->gfx()->print(hal->getLocalWeekday(&tmpCursor));
    hal->gfx()->setTextCursor(120, 190);
    hal->gfx()->print(String(hal->environment->getStepsOnDay(tmpCursor, true))); // lastweek(before 7 day)
    hal->gfx()->setTextCursor(120, 215);
    hal->gfx()->print(String(hal->environment->getStepsTotalWeek() / 7) + String("/") + String(hal->environment->getStepsTotalWeek())); // Avg/Total
    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextCursor(120, 205);
    hal->gfx()->print(String(hal->environment->getStepsOnDay(tmpCursor))); // Big font Fitness value
}

void OswAppStepStats::setup()
{
    OswHal *hal = OswHal::getInstance();
    uint32_t weekDay = 0;
    uint32_t dayOfMonth = 0;
    hal->getLocalDate(&dayOfMonth, &weekDay);
    cursorPos = weekDay;
}
void OswAppStepStats::loop()
{
    OswHal *hal = OswHal::getInstance();
    if (hal->btnHasGoneDown(BUTTON_3))
    {
        this->cursorPos = this->cursorPos + 1 > 6 ? 6 : this->cursorPos + 1;
    }
    if (hal->btnHasGoneDown(BUTTON_2))
    {
        this->cursorPos = this->cursorPos - 1 < 0 ? 0 : this->cursorPos - 1;
    }
    showStickChart();
    hal->requestFlush();
}

void OswAppStepStats::stop() {}
#endif