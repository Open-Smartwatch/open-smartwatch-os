#ifdef OSW_FEATURE_STATS_STEPS

#include "./apps/tools/OswAppKcalStats.h"
#include "./apps/watchfaces/OswAppWatchfaceFitness.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_pins.h>

#define MAX_VALUE 85
#define MIN_VALUE 135

/*!
 @param type 0 == Graph, 1 == step, 2 == Distance, 3 == Kcalorie
 @param cursor The value that the cursor points to.
 @param cursorValue The cursor value you want to know
 */
void OswAppKcalStats::readyValue(int type, uint8_t cursor, uint32_t* cursorValue) {
  OswHal* hal = OswHal::getInstance();
  uint32_t day = 0;
  uint32_t weekday = 0;
  hal->getLocalDate(&day, &weekday);
  int curvedIdx = 0;
  uint8_t graphRange = MIN_VALUE - MAX_VALUE;
  uint16_t kcalGoal = OswConfigAllKeys::kcalPerDay.get();
  for (int Index = ((weekday + 1 > 6) ? 0 : weekday + 1); curvedIdx < 7; Index++, curvedIdx++) {
    if (Index > 6) Index = 0;
    uint32_t weekDayValue = 0;
    if (type == 0) { // Graph
      weekDayValue = OswAppWatchfaceFitness::calculateKcalorie(hal->environment->getStepsOnDay(Index));
      weekDayValue = ((float)(weekDayValue > kcalGoal ? kcalGoal : weekDayValue) / kcalGoal) * graphRange;
    } else if (type == 1) { // Step
      weekDayValue = hal->environment->getStepsOnDay(Index);
    } else if (type == 2) { // Distance
      weekDayValue = OswAppWatchfaceFitness::calculateDistance(hal->environment->getStepsOnDay(Index));
    } else if (type == 3) { // Calorie
      weekDayValue = OswAppWatchfaceFitness::calculateKcalorie(hal->environment->getStepsOnDay(Index));
    }
    weekValue[curvedIdx] = weekDayValue;
  }
  if (cursorValue != nullptr){
    *cursorValue = weekValue[cursor];
  }
}

void OswAppKcalStats::drawCurvedChart() {
  OswHal* hal = OswHal::getInstance();

  uint8_t x1, y1 , x2 , y2 = 0;
  uint8_t interval = 25;

  for (uint8_t Index = 0; Index < 6; Index++) {
    
    x1 = ((DISP_W / 2) - interval * 3) + Index * interval;
    y1 = MIN_VALUE - weekValue[Index];
    x2 = x1 + interval;
    y2 = MIN_VALUE - weekValue[Index + 1];

    if (Index == this->cursorPos || ( this->cursorPos == 6 && Index == 5)) {
      hal->gfx()->drawThickTick(this->cursorPos == 6 && Index == 5?x2:x1, 140, 0, 60, 0, 3, ui->getForegroundColor());
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

  OswAppKcalStats::readyValue();
  OswAppKcalStats::drawCurvedChart();

  uint8_t coordX = 30;
  hal->gfx()->drawThickTick(coordX, 150, 0, 240 - (coordX * 2), 90, 2, ui->getPrimaryColor());
  hal->gfx()->drawLine(DISP_W / 2, 150 + 15, 120, 220, ui->getPrimaryColor());  

  // Data info
  hal->gfx()->setTextSize(1);
  hal->gfx()->setTextCenterAligned();
  hal->gfx()->setTextBottomAligned();
  hal->gfx()->setTextColor(ui->getForegroundColor());

  hal->gfx()->setTextCenterAligned();
  hal->gfx()->setTextCursor(80, 140 + 25);
  hal->gfx()->print(LANG_KCAL_AVG);  // total step counter
  hal->gfx()->setTextCursor(160, 140 + 25);
  hal->gfx()->print(LANG_KCAL_TODAY);  // total step counter

  hal->gfx()->setTextRightAligned();
  hal->gfx()->setTextCursor(DISP_W / 2 - 7, 160 + 25);
  hal->gfx()->print(String(OswAppWatchfaceFitness::calculateDistance(hal->environment->getStepsTotal()) / 7 ));
  hal->gfx()->setTextCursor(DISP_W / 2 - 7, 160 + 25 + 10);
  hal->gfx()->print(hal->environment->getStepsTotal()/7);  // total step counter
  hal->gfx()->setTextCursor(DISP_W / 2 - 7, 160 + 25 + 20);
  hal->gfx()->print(OswAppWatchfaceFitness::calculateKcalorie(hal->environment->getStepsTotal())/7);  // total step counter
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(DISP_W / 2 + 7, 160 + 25);
  uint32_t cursorBox = 0;
  readyValue(2, this->cursorPos, &cursorBox); 
  hal->gfx()->print(cursorBox+String(" m"));
  hal->gfx()->setTextCursor(DISP_W / 2 + 7, 160 + 25 + 10);
  readyValue(1,this->cursorPos,&cursorBox);
  hal->gfx()->print(cursorBox + String(" ") + String(LANG_KCAL_STEP));  // total step counter
  hal->gfx()->setTextCursor(DISP_W / 2 + 7, 160 + 25 + 20);
  readyValue(3,this->cursorPos,&cursorBox);
  hal->gfx()->print(cursorBox+String(" kcal"));  // total step counter
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