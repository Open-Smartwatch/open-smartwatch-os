
#include "osw_app_pauls_watchface.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

#include "bma400_defs.h"

void drawBananaWatch(OswHal* hal, Graphics2D* gfx2d) {
  gfx2d->drawArc(120, 120, 0, 360, 90, 113, 5, rgb565(32, 32, 32));
  // gfx2d.drawMinuteTicks(120, 120, 116, 50, rgb565(255, 0, 0));
  gfx2d->drawHourTicks(120, 120, 117, 107, rgb565(128, 128, 128));

  uint32_t steps = hal->getStepCount();

  gfx2d->drawArc(120, 120, 0, 360, 180, 93, 7, changeColor(rgb565(210, 50, 66), 0.25));
  gfx2d->drawArc(120, 120, 0, steps % 360, 180, 93, 7, dimColor(rgb565(210, 50, 66), 25));
  gfx2d->drawArc(120, 120, 0, steps % 360, 180, 93, 6, rgb565(210, 50, 66));

  gfx2d->drawArc(120, 120, 0, 360, 180, 75, 7, changeColor(rgb565(117, 235, 10), 0.25));
  gfx2d->drawArc(120, 120, 0, (steps / 360) % 360, 180, 75, 7, dimColor(rgb565(117, 235, 10), 25));
  gfx2d->drawArc(120, 120, 0, (steps / 360) % 360, 180, 75, 6, rgb565(117, 235, 10));

  gfx2d->drawArc(120, 120, 0, 360, 180, 57, 7, changeColor(rgb565(25, 193, 202), 0.25));
  gfx2d->drawArc(120, 120, 0, 32, 180, 57, 7, dimColor(rgb565(25, 193, 202), 25));
  gfx2d->drawArc(120, 120, 0, 32, 180, 57, 6, rgb565(25, 193, 202));

  static uint32_t ticks = 0;
  ticks++;
  float deltaAngle = ticks;
  // hours
  gfx2d->drawThickTick(120, 120, 0, 16 + deltaAngle / (3600), -66, 1, rgb565(255, 255, 255));
  gfx2d->drawThickTick(120, 120, 16, 60, -66 + deltaAngle / (3600), 4, rgb565(255, 255, 255));

  // minutes
  gfx2d->drawThickTick(120, 120, 0, 16, 45 + deltaAngle / (60), 1, rgb565(255, 255, 255));
  gfx2d->drawThickTick(120, 120, 16, 105, 45 + deltaAngle / (60), 4, rgb565(255, 255, 255));

  // seconds
  gfx2d->fillCircle(120, 120, 3, rgb565(255, 0, 0));
  gfx2d->drawThickTick(120, 120, 0, 16, 0 + deltaAngle, 1, rgb565(255, 0, 0));
  gfx2d->drawThickTick(120, 120, 0, 110, 180 + deltaAngle, 1, rgb565(255, 0, 0));
}

void OswAppPaulsWatchface::loop(OswHal* hal) {
  static long loopCount = 0;
  loopCount++;
  // drawColors(hal);

  uint8_t r = map((long)hal->getAccelerationX(), 0, 1024, 0, 255);
  uint8_t g = map((long)hal->getAccelerationY(), 0, 1024, 0, 255);
  uint8_t b = map((long)hal->getAccelerationZ(), 0, 1024, 0, 255);

  uint16_t color = rgb565(r, g, b);

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

  drawBananaWatch(hal, hal->getCanvas()->getGraphics2D());

  if (hal->btn3Down()) {
    // deep sleep after 30 seconds
    Serial.println("good night");
    hal->getCanvas()->getGraphics2D()->fill(rgb565(0, 0, 0));
    hal->flushCanvas();
    hal->setBrightness(10);
    delay(500);
    hal->deepSleep();
  }
}
