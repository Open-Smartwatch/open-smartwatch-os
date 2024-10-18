#ifdef OSW_FEATURE_STATS_STEPS

#include "./apps/tools/OswAppStepStats.h"
#include "./apps/watchfaces/OswAppWatchfaceFitness.h"

#include <gfx_util.h>
#include <OswAppV1.h>
#include <osw_hal.h>
#include <osw_pins.h>

void OswAppStepStats::drawChart() {
    OswHal* hal = OswHal::getInstance();
    uint8_t chartStickHeight = 55;
    uint8_t interval = 20;
    uint16_t goalValue = OswConfigAllKeys::stepsPerDay.get();

    for (uint8_t index = 0; index < 7; index++) {
        unsigned int weekDayStep = hal->environment()->getStepsOnDay(index);
        unsigned short chartStickValue = ((float)(weekDayStep > goalValue ? goalValue : weekDayStep) / goalValue) * chartStickHeight;

        uint16_t barColor = (unsigned int) OswConfigAllKeys::stepsPerDay.get() <= weekDayStep ? ui->getSuccessColor() : changeColor(ui->getSuccessColor(), 2.85);

        chartStickValue = chartStickValue < 2 ? 0 : chartStickValue;

        if (index == cursorPos) {
            hal->gfx()->drawThickTick(60 + index * interval, 147, 0, chartStickHeight, 0, 5, ui->getForegroundColor());
        }
        hal->gfx()->drawThickTick(60 + index * interval, 147, 0, chartStickValue, 0, 3, barColor, true);
    }
}

void OswAppStepStats::drawInfoPanel(OswUI* ui, uint32_t pos, uint32_t lastWeekData, uint32_t todayData, uint32_t average, uint32_t total,const String& unit) {
    OswHal* hal = OswHal::getInstance();

    uint8_t coord_X = 30;

    hal->gfx()->drawThickTick(coord_X, 150, 0, DISP_W - (coord_X * 2), 90, 2, ui->getPrimaryColor());
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextBottomAligned();
    hal->gfx()->setTextCursor(DISP_W / 2, 170);
    try {
        const char* weekday = hal->getWeekDay.at(pos);
        hal->gfx()->print(weekday);
    } catch (const std::out_of_range& ignore) {
        OSW_LOG_E("getWeekDay is out of range: ", ignore.what());
    }
    hal->gfx()->setTextCursor(DISP_W / 2, 190);
    hal->gfx()->print(String(lastWeekData)); // lastweek(before 7 day)
    hal->gfx()->setTextCursor(DISP_W / 2, 215);
    hal->gfx()->print(String(average) + String("/") + String(total)); // Avg/Total
    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextCursor(DISP_W / 2, 205);
    hal->gfx()->print(String(todayData) + unit); // Big font Fitness value
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
    OswAppStepStats::drawInfoPanel(ui,(uint32_t)cursorPos, hal->environment()->getStepsOnDay((uint32_t)cursorPos, true), hal->environment()->getStepsOnDay((uint32_t)cursorPos), hal->environment()->getStepsAverage(), hal->environment()->getStepsTotalWeek());
}

void OswAppStepStats::setup() {
    OswHal* hal = OswHal::getInstance();
    OswDate oswDate = { };
    hal->getLocalDate(oswDate);
    cursorPos = oswDate.weekDay;
}
void OswAppStepStats::loop() {
    OswHal* hal = OswHal::getInstance();
    if (hal->btnHasGoneDown(BUTTON_3)) {
        this->cursorPos = this->cursorPos + 1 > 6 ? 6 : this->cursorPos + 1;
    }
    if (hal->btnHasGoneDown(BUTTON_2)) {
        this->cursorPos = this->cursorPos - 1 < 0 ? 0 : this->cursorPos - 1;
    }
    showStickChart();
}

#ifdef OSW_EMULATOR
#include "imgui.h"

void OswAppStepStats::loopDebug() {
    ImGui::Begin("Debug: OswAppStepStats");
    ImGui::InputScalar("cursorPos", ImGuiDataType_U8, &this->cursorPos);
    ImGui::End();
}
#endif

void OswAppStepStats::stop() {}
#endif
