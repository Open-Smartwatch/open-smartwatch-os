#include "./apps/watchfaces/OswAppWatchfaceDual.h"

#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <time.h>

#include "./apps/watchfaces/OswAppWatchface.h"
#include "./apps/watchfaces/OswAppWatchfaceDigital.h"

void OswAppWatchfaceDual::drawProgressBar(uint8_t cx, uint8_t cy, uint8_t jump, uint8_t length, uint8_t value,float angle, uint8_t radius, uint16_t color, int* goal) {
  OswHal* hal = OswHal::getInstance();
  hal->gfx()->drawThickTick(cx, cy, jump, length, angle, radius,  changeColor(color, 0.25));
  hal->gfx()->drawThickTick(cx, cy, jump, value, angle, radius, goal==nullptr?color:*goal<value ? ui->getSuccessColor():color, true);
}

void OswAppWatchfaceDual::drawAnim() {
  OswHal* hal = OswHal::getInstance();

  uint8_t barWidth = 140;

  uint32_t Hs, Ms, Ss = 0;
  hal->getLocalTime(&Hs,&Ms,&Ss);
  uint32_t Val = Ss;  // virtual step simulation
  uint16_t box_Height = ((float)(Val > 60 ? 60 : Val) / 60) * barWidth;
  box_Height = box_Height < 2 ? 0 : box_Height;

  uint8_t x_point = (240 - barWidth) / 2;
  uint8_t y_level = 120;
  uint8_t r = 1.5;
  drawProgressBar(x_point, y_level - 1, 0, barWidth, box_Height,90,r, ui->getPrimaryColor());

  // hal->gfx()->drawArc(120, 120 - 1, 0, 360, 90, 115, 1, changeColor(rgb565(65, 125, 125), 3.25));
  // // hal->gfx()->drawArc(120, 120, 180, 180+32, 90, 57, 7, dimColor(rgb565(25, 193, 202), 25));
  // hal->gfx()->drawArc(120, 120 - 1, 0, (360.0 * (float)(Val % 60) / (float)60), 90, 115, 1, rgb565(65, 125, 125), true);
}

void OswAppWatchfaceDual::setup() {}

void OswAppWatchfaceDual::loop() {
  OswHal* hal = OswHal::getInstance();
  if (hal->btnHasGoneDown(BUTTON_3)) {
    hal->increaseBrightness(25);
  }
  if (hal->btnHasGoneDown(BUTTON_2)) {
    hal->decreaseBrightness(25);
  }

  hal->gfx()->fill(ui->getBackgroundColor());

  // Set Dual Size
  hal->gfx()->setTextSize(2);
  uint8_t mid_little = hal->gfx()->getTextOfsetRows(0.5);
  uint8_t mid = hal->gfx()->getTextOfsetRows(2);

  OswAppWatchfaceDigital::digitalWatch(OswConfigAllKeys::timeZone.get() ,1, 120 - mid_little, 120 - mid);
  OswAppWatchfaceDigital::digitalWatch(OswConfigAllKeys::dualTimeZone.get() ,1, 120 + mid_little, 120 + mid);

  drawAnim();

#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
  OswAppWatchfaceDigital::drawSteps();
#endif

  hal->requestFlush();
}

void OswAppWatchfaceDual::stop() {
  // OswHal::getInstance()->disableDisplayBuffer();
}
