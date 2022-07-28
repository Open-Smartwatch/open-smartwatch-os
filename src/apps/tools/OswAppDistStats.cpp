#ifdef OSW_FEATURE_STATS_STEPS

#include "./apps/tools/OswAppDistStats.h"
#include "./apps/watchfaces/OswAppWatchfaceFitness.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_pins.h>

void OswAppDistStats::drawChart() {
    OswHal* hal = OswHal::getInstance();
    uint8_t chartStickHeight = 55;
    uint8_t interval = 20;
    uint16_t goalValue = OswConfigAllKeys::distPerDay.get();

    uint32_t weekDay = 0;
    uint32_t dayOfMonth = 0;
    hal->getLocalDate(&dayOfMonth, &weekDay);

    for (uint8_t index = 0; index < 7; index++) {
        uint32_t weekDayDist = OswAppWatchfaceFitness::calculateDistance(hal->environment->getStepsOnDay(index));
        uint16_t chartStickValue = ((float)(weekDayDist > goalValue ? goalValue : weekDayDist) / goalValue) * chartStickHeight;

        uint16_t barColor = OswConfigAllKeys::distPerDay.get() <= weekDayDist ? ui->getSuccessColor() : changeColor(ui->getSuccessColor(),2.85);

        chartStickValue = chartStickValue < 2 ? 0 : chartStickValue;

        if (index == cursorPos) {
            hal->gfx()->drawThickTick(60 + index * interval, 147, 0, chartStickHeight, 0, 5, ui->getForegroundColor());
        }
        hal->gfx()->drawThickTick(60 + index * interval, 147, 0, chartStickValue, 0, 3, barColor, true);
    }
}

void OswAppDistStats::showStickChart() {
    OswHal* hal = OswHal::getInstance();

    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextCenterAligned();

    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextCursor(DISP_H / 2, 60);
    hal->gfx()->setTextColor(ui->getForegroundColor());
    hal->gfx()->print(LANG_DISTSTATS_TITLE);

    OswAppDistStats::drawChart();

    uint8_t coord_x = 30;

    hal->gfx()->drawThickTick(coord_x, 150, 0, DISP_W - (coord_x * 2), 90, 2, ui->getPrimaryColor());
    uint32_t tmpCursor = cursorPos;
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextBottomAligned();
    hal->gfx()->setTextCursor(DISP_W/2, 170);
    hal->gfx()->print(hal->getLocalWeekday(&tmpCursor));
    hal->gfx()->setTextCursor(DISP_W/2, 190);
    hal->gfx()->print(String(OswAppWatchfaceFitness::calculateDistance(hal->environment->getStepsOnDay(tmpCursor,true))) ); // lastweek(before 7 day)
    hal->gfx()->setTextCursor(DISP_W/2, 215);
    hal->gfx()->print(String(OswAppWatchfaceFitness::calculateDistance(hal->environment->getStepsAverage())) + String("/") + String(OswAppWatchfaceFitness::calculateDistance(hal->environment->getStepsTotalWeek()))); // Avg/Total
    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextCursor(DISP_W/2, 205);
    hal->gfx()->print(String(OswAppWatchfaceFitness::calculateDistance(hal->environment->getStepsOnDay(tmpCursor)) + String(" m"))); // Big font Fitness value
}

void OswAppDistStats::setup() {
    OswHal* hal = OswHal::getInstance();
    uint32_t weekDay = 0;
    uint32_t dayOfMonth = 0;
    hal->getLocalDate(&dayOfMonth, &weekDay);
    cursorPos = weekDay;
}
void OswAppDistStats::loop() {
    OswHal* hal = OswHal::getInstance();
    if (hal->btnHasGoneDown(BUTTON_3)) {
        this->cursorPos = this->cursorPos + 1 > 6 ? 6 : this->cursorPos + 1;
    }
    if (hal->btnHasGoneDown(BUTTON_2)) {
        this->cursorPos = this->cursorPos - 1 < 0 ? 0 : this->cursorPos - 1;
    }

    showStickChart();
    hal->requestFlush();
}

void OswAppDistStats::stop() {}
#endif