
#include "./apps/tools/OswAppKcalStats.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_pins.h>

#define MAX_VALUE 85
#define MIN_VALUE 135

// use queue
void OswAppKcalStats::readyValue() {
  OswHal* hal = OswHal::getInstance();
  uint32_t day = 0;
  uint32_t weekday = 0;
  hal->getLocalDate(&day, &weekday);
  int a = 0;

  for (int i = ((weekday + 1 > 6) ? 0 : weekday + 1); i != weekday; i++,a++) {
    if (i > 6) i = 0;
    weekValue[a] = hal->environment->getStepsOnDay(i);
  }
}

void OswAppKcalStats::drawCurvedChart() {
  OswHal* hal = OswHal::getInstance();

  uint8_t x1 = 60;
  uint8_t y1 = 120;
  uint8_t x2 = 180;
  uint8_t y2 = 120;
  uint8_t type_distance = 25;

  for (uint8_t i = 0; i < 6; i++) {
    x1 = ((240 / 2) - type_distance * 3) + i * type_distance;
    y1 = weekValue[i];
    x2 = x1 + type_distance;
    y2 = weekValue[i + 1];
    if (i == this->cursorPos) {
      hal->gfx()->drawThickTick(((240 / 2) - type_distance * 3) + i * type_distance, 140, 0, 60, 0, 3, rgb888to565(rgb888(235, 235, 235)));
    }
    hal->gfx()->drawLine(x1, y1, x2, y2, rgb565(255, 0, 0));  // long front

    hal->gfx()->fillCircle(x1, y1, 2.5, rgb565(55, 222, 55));

    if (i == 5) hal->gfx()->fillCircle(x2, y2, 2.5, rgb565(55, 222, 55));
  }
}

void OswAppKcalStats::showCurvedChart() {
  OswHal* hal = OswHal::getInstance();

  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextCenterAligned();

  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextCursor(DISP_H / 2, 60);
  hal->gfx()->setTextColor(rgb565(255, 255, 255));
  hal->gfx()->print("Kcal stats");

  OswAppKcalStats::readyValue();
  
  OswAppKcalStats::drawCurvedChart();

  uint8_t coord_x = 30;
  hal->gfx()->drawThickTick(coord_x, 150, 0, 240 - (coord_x * 2), 90, 2, rgb888to565(rgb888(135, 135, 135)));
  hal->gfx()->drawLine(120, 150 + 15, 120, 220, rgb888to565(rgb888(135, 135, 135)));  // long front

  // hal->gfx()->drawLine(80, 150 + 20, 80, 200, rgb888to565(rgb888(135, 135, 135)));    // long front
  // hal->gfx()->drawLine(160, 150 + 20, 160, 200, rgb888to565(rgb888(135, 135, 135)));  // long front

  // labels
  hal->gfx()->setTextCenterAligned();  // horiz.
  hal->gfx()->setTextBottomAligned();
  hal->gfx()->setTextSize(1);
  hal->gfx()->setTextCursor(240 / 2, 180 + 20);
  hal->gfx()->setTextColor(rgb565(255, 255, 255));

  hal->gfx()->setTextCenterAligned();
  hal->gfx()->setTextCursor(80, 140 + 25);
  hal->gfx()->print("Avg");  // total step counter
  hal->gfx()->setTextCursor(160, 140 + 25);
  hal->gfx()->print("Today");  // total step counter

  hal->gfx()->setTextRightAligned();
  hal->gfx()->setTextCursor(120 - 7, 160 + 25);
  hal->gfx()->print("1235Km");  // total step counter
  hal->gfx()->setTextCursor(120 - 7, 160 + 25 + 10);
  hal->gfx()->print("13425 Step");  // total step counter
  hal->gfx()->setTextCursor(120 - 7, 160 + 25 + 20);
  hal->gfx()->print("4125 Dist");  // total step counter
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(120 + 7, 160 + 25);
  hal->gfx()->print("477 Kcal");
  hal->gfx()->setTextCursor(120 + 7, 160 + 25 + 10);
  hal->gfx()->print("23425 Step");  // total step counter
  hal->gfx()->setTextCursor(120 + 7, 160 + 25 + 20);
  hal->gfx()->print("125 Dist");  // total step counter
                                  //    hal->gfx()->print((int)(x * 0.04));  // today step counter
}

void OswAppKcalStats::setup() {}

void OswAppKcalStats::loop() {
  OswHal* hal = OswHal::getInstance();
  if (hal->btnHasGoneDown(BUTTON_3)) {
    this->cursorPos = this->cursorPos + 1 > 6 ? 6 : this->cursorPos +1;
  }
  if (hal->btnHasGoneDown(BUTTON_2)) {
      this->cursorPos = this->cursorPos - 1 < 0 ? 0 : this->cursorPos -1;
  }
  hal->gfx()->fill(ui->getBackgroundColor());

  // to better understand the accelerometer values use the debug function
  // debug(hal);

  showCurvedChart();

  hal->requestFlush();
}

void OswAppKcalStats::stop() {}
