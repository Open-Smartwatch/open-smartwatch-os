
#include "./apps/main/watchface.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

#include "bma400_defs.h"

#define COLOR_RED rgb565(210, 50, 66)
#define COLOR_GREEN rgb565(117, 235, 10)
#define COLOR_BLUE rgb565(25, 193, 202)

void drawWatch(OswHal* hal, Graphics2D* gfx2d) {
  gfx2d->drawArc(120, 120, 0, 360, 90, 113, 5, rgb565(32, 32, 32));
  // gfx2d.drawMinuteTicks(120, 120, 116, 50, rgb565(255, 0, 0));
  gfx2d->drawHourTicks(120, 120, 117, 107, rgb565(128, 128, 128));

  uint32_t steps = hal->getStepCount();

  gfx2d->drawArc(120, 120, 0, 360, 180, 93, 7, changeColor(COLOR_RED, 0.25));
  gfx2d->drawArc(120, 120, 0, steps % 360, 180, 93, 7, dimColor(COLOR_RED, 25));
  gfx2d->drawArc(120, 120, 0, steps % 360, 180, 93, 6, COLOR_RED);

  gfx2d->drawArc(120, 120, 0, 360, 180, 75, 7, changeColor(COLOR_GREEN, 0.25));
  gfx2d->drawArc(120, 120, 0, (steps / 360) % 360, 180, 75, 7, dimColor(COLOR_GREEN, 25));
  gfx2d->drawArc(120, 120, 0, (steps / 360) % 360, 180, 75, 6, COLOR_GREEN);

  float bat = hal->getBatteryPercent() * 3.6;

  gfx2d->drawArc(120, 120, 0, 360, 180, 57, 7, changeColor(COLOR_BLUE, 0.25));
  gfx2d->drawArc(120, 120, 0, bat, 180, 57, 7, dimColor(COLOR_BLUE, 25));
  gfx2d->drawArc(120, 120, 0, bat, 180, 57, 6, COLOR_BLUE);

  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;
  hal->getTime(&hour, &minute, &second);

  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);

  // hours
  gfx2d->drawThickTick(120, 120, 0, 16, 360.0 / 12.0 * (1.0 * hour + minute / 60.0), 1, rgb565(255, 255, 255));
  gfx2d->drawThickTick(120, 120, 16, 60, 360.0 / 12.0 * (1.0 * hour + minute / 60.0), 4, rgb565(255, 255, 255));

  // minutes
  gfx2d->drawThickTick(120, 120, 0, 16, 360.0 / 60.0 * (1.0 * minute + second / 60.0), 1, rgb565(255, 255, 255));
  gfx2d->drawThickTick(120, 120, 16, 105, 360.0 / 60.0 * (1.0 * minute + second / 60.0), 4, rgb565(255, 255, 255));

  // seconds
  gfx2d->fillCircle(120, 120, 3, rgb565(255, 0, 0));
  gfx2d->drawThickTick(120, 120, 0, 16, 360.0 / 60.0 * second, 1, rgb565(255, 0, 0));
  gfx2d->drawThickTick(120, 120, 0, 110, 360.0 / 60.0 * second, 1, rgb565(255, 0, 0));
}

void OswAppWatchface::setup(OswHal* hal) {
  // hal->enableDisplayBuffer();
}

void OswAppWatchface::loop(OswHal* hal) {
  static long loopCount = 0;
  loopCount++;
  // drawColors(hal);

  // uint8_t r = map((long)hal->getAccelerationX(), 0, 1024, 0, 255);
  // uint8_t g = map((long)hal->getAccelerationY(), 0, 1024, 0, 255);
  // uint8_t b = map((long)hal->getAccelerationZ(), 0, 1024, 0, 255);

  // uint16_t color = rgb565(r, g, b);

  hal->getCanvas()->getGraphics2D()->fill(rgb565(0, 0, 0));

  // hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
  // hal->getCanvas()->setCursor(60, 100);
  // hal->getCanvas()->setTextSize(4);
  // hal->getCanvas()->print(hal->getStepCount(), 10);
  // hal->getCanvas()->setCursor(60, 140);
  // switch (hal->getActivityMode()) {
  //   case BMA400_STILL_ACT:
  //     hal->getCanvas()->print("still");
  //     break;
  //   case BMA400_WALK_ACT:
  //     hal->getCanvas()->print("walk ");
  //     break;
  //   case BMA400_RUN_ACT:
  //     hal->getCanvas()->print("run  ");
  //     break;
  //   default:
  //     hal->getCanvas()->print(hal->getActivityMode(), 10);
  // }

  drawWatch(hal, hal->getCanvas()->getGraphics2D());

  // hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
  // hal->getCanvas()->setCursor(60, 100);
  // hal->getCanvas()->setTextSize(2);
  // if (hal->isCharging()) {
  //   hal->getCanvas()->print("charging");
  // } else {
  //   hal->getCanvas()->print("discharging");
  // }

  hal->requestFlush();
}

void OswAppWatchface::stop(OswHal* hal) {
  // hal->disableDisplayBuffer();
}
