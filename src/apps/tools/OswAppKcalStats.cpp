#ifdef OSW_FEATURE_STATS_STEPS

#include "./apps/tools/OswAppKcalStats.h"
#include "./apps/watchfaces/OswAppWatchfaceFitness.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_pins.h>

#define MAX_VALUE 85
#define MIN_VALUE 135

uint32_t findCursorWeekDay(uint8_t Index) {  // Show the day of the week that cursor (Dynamic weekDay--info)
    OswHal* hal = OswHal::getInstance();
    uint32_t d, wD = 0;
    hal->getLocalDate(&d, &wD);
    int cursorWeekDay = wD - (6 - Index);
    int findWeekDay = (cursorWeekDay >= 0) ? cursorWeekDay : (cursorWeekDay + 7);
    uint32_t fWD = findWeekDay;
    return fWD;
}

uint8_t convertValue(uint32_t value,uint16_t goal) {
    uint8_t graphRange = MIN_VALUE - MAX_VALUE;
    uint8_t cvValue = ((float)(value > goal ? goal : value) / goal) * graphRange;
    return cvValue;
}

void OswAppKcalStats::drawCurvedChart() {
    OswHal* hal = OswHal::getInstance();

    uint8_t x1, y1, x2, y2, y1Val, y2Val = 0;
    uint8_t interval = 25;

    for (uint8_t Index = 0; Index < 6; Index++) {

        x1 = ((DISP_W / 2) - interval * 3) + Index * interval;
        y1Val = convertValue(OswAppWatchfaceFitness::calculateKcalorie(hal->environment->getStepsOnDay(findCursorWeekDay(Index))),OswConfigAllKeys::kcalPerDay.get());
        y1 = MIN_VALUE - y1Val;
        x2 = x1 + interval;
        y2Val = convertValue(OswAppWatchfaceFitness::calculateKcalorie(hal->environment->getStepsOnDay(findCursorWeekDay(Index+1))),OswConfigAllKeys::kcalPerDay.get());
        y2 = MIN_VALUE - y2Val;

        if (Index == this->cursorPos || ( this->cursorPos == 6 && Index == 5)) {
            hal->gfx()->drawThickTick(this->cursorPos == 6 && Index == 5 ? x2 : x1, 140, 0, 60, 0, 3, ui->getForegroundColor());
        }

        hal->gfx()->drawLine(x1, y1, x2, y2, changeColor(ui->getSuccessColor(),2.25));  // first-second Coord
        hal->gfx()->fillCircle(x1, y1, 2.5,ui->getSuccessColor() ); // draw circle on the first coord

        // last coord
        if (Index == 5) {
            hal->gfx()->fillCircle(x2, y2, 2.5,ui->getSuccessColor() );
        }
    }
}

void OswAppKcalStats::showCurvedChart() {
    OswHal* hal = OswHal::getInstance();

    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextCenterAligned();

    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextCursor(DISP_H / 2, 60);
    hal->gfx()->setTextColor(ui->getForegroundColor());
    hal->gfx()->print(LANG_KCAL_TITLE);

    OswAppKcalStats::drawCurvedChart();

    uint8_t coordX = 30;
    hal->gfx()->drawThickTick(coordX, 150, 0, 240 - (coordX * 2), 90, 2, ui->getPrimaryColor());
    hal->gfx()->drawLine(DISP_W / 2, 165, 120, 220, ui->getPrimaryColor());

    // Data info
    uint32_t wDay = findCursorWeekDay(this->cursorPos);
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextBottomAligned();
    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextCursor(80, 165);
    hal->gfx()->print(LANG_KCAL_AVG);
    hal->gfx()->setTextCursor(160, 165);
    hal->gfx()->print(hal->getLocalWeekday(&wDay));
    hal->gfx()->setTextRightAligned();
    hal->gfx()->setTextCursor(DISP_W / 2 - 7, 185);
    hal->gfx()->print(String(OswAppWatchfaceFitness::calculateDistance(hal->environment->getStepsTotalWeek()) / 7 ));
    hal->gfx()->setTextCursor(DISP_W / 2 - 7, 195);
    hal->gfx()->print(hal->environment->getStepsTotalWeek()/7);
    hal->gfx()->setTextCursor(DISP_W / 2 - 7, 205);
    hal->gfx()->print(OswAppWatchfaceFitness::calculateKcalorie(hal->environment->getStepsTotalWeek())/7);
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextCursor(DISP_W / 2 + 7, 185);
    hal->gfx()->print(OswAppWatchfaceFitness::calculateDistance(hal->environment->getStepsOnDay(wDay))+String(" m"));
    hal->gfx()->setTextCursor(DISP_W / 2 + 7, 195);
    hal->gfx()->print(hal->environment->getStepsOnDay(wDay) + String(" ") + String(LANG_KCAL_STEP));
    hal->gfx()->setTextCursor(DISP_W / 2 + 7, 205);
    hal->gfx()->print(OswAppWatchfaceFitness::calculateKcalorie(hal->environment->getStepsOnDay(wDay))+String(" kcal"));
}

void OswAppKcalStats::setup() {}

void OswAppKcalStats::loop() {
    OswHal* hal = OswHal::getInstance();
    if (hal->btnHasGoneDown(BUTTON_3)) {
        this->cursorPos = this->cursorPos + 1 > 6 ? 6 : this->cursorPos + 1;
    }
    if (hal->btnHasGoneDown(BUTTON_2)) {
        this->cursorPos = this->cursorPos - 1 < 0 ? 0 : this->cursorPos - 1;
    }
    hal->gfx()->fill(ui->getBackgroundColor());

    showCurvedChart();

    hal->requestFlush();
}

void OswAppKcalStats::stop() {}
#endif