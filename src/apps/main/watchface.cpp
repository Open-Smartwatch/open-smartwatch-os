
#include "./apps/main/watchface.h"
// #define GIF_BG

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config.h>
#include <osw_config_types.h>
#include <osw_hal.h>

#ifdef GIF_BG
#include "./apps/_experiments/gif_player.h"
#endif

#include "bma400_defs.h"

#define COLOR_RED rgb565(210, 50, 66)
#define COLOR_GREEN rgb565(117, 235, 10)
#define COLOR_BLUE rgb565(25, 193, 202)
#define COLOR_WOOD rgb565(179, 107, 0)

void drawWatch(OswHal* hal, Graphics2D* gfx2d) {
#ifndef GIF_BG
  gfx2d->drawArc(120, 120, 0, 360, 90, 113, 5, rgb565(32, 32, 32));
#endif
  // gfx2d.drawMinuteTicks(120, 120, 116, 50, rgb565(255, 0, 0));
  gfx2d->drawHourTicks(120, 120, 117, 107, rgb565(128, 128, 128));

  uint32_t steps = hal->getStepCount();
  uint16_t primaryColor = OswConfigAllKeys::appWTFprimaryColor
                              .get();  // TODO as this accesses the nvs, it should only be loaded inside the setup()
#ifndef GIF_BG
  gfx2d->drawArc(120, 120, 0, 360, 180, 93, 7, changeColor(primaryColor, 0.25));
  gfx2d->drawArc(120, 120, 0, steps % 360, 180, 93, 7, dimColor(primaryColor, 25));
#endif
  gfx2d->drawArc(120, 120, 0, steps % 360, 180, 93, 6, primaryColor);

  // below two arcs take too long to draw

  // gfx2d->drawArc(120, 120, 0, 360, 180, 75, 7, changeColor(COLOR_GREEN, 0.25));
  // gfx2d->drawArc(120, 120, 0, (steps / 360) % 360, 180, 75, 7, dimColor(COLOR_GREEN, 25));
  // gfx2d->drawArc(120, 120, 0, (steps / 360) % 360, 180, 75, 6, COLOR_GREEN);

  // float bat = hal->getBatteryPercent() * 3.6;

  // gfx2d->drawArc(120, 120, 0, 360, 180, 57, 7, changeColor(COLOR_BLUE, 0.25));
  // gfx2d->drawArc(120, 120, 0, bat, 180, 57, 7, dimColor(COLOR_BLUE, 25));
  // gfx2d->drawArc(120, 120, 0, bat, 180, 57, 6, COLOR_BLUE);

  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;
  hal->getLocalTime(&hour, &minute, &second);

  // hours
  gfx2d->drawThickTick(120, 120, 0, 16, 360.0 / 12.0 * (1.0 * hour + minute / 60.0), 1, rgb565(255, 255, 255));
  gfx2d->drawThickTick(120, 120, 16, 60, 360.0 / 12.0 * (1.0 * hour + minute / 60.0), 4, rgb565(255, 255, 255));

  // minutes
  gfx2d->drawThickTick(120, 120, 0, 16, 360.0 / 60.0 * (1.0 * minute + second / 60.0), 1, rgb565(255, 255, 255));
  gfx2d->drawThickTick(120, 120, 16, 105, 360.0 / 60.0 * (1.0 * minute + second / 60.0), 4, rgb565(255, 255, 255));

#ifndef GIF_BG
  // seconds
  gfx2d->fillCircle(120, 120, 3, rgb565(255, 0, 0));
  gfx2d->drawThickTick(120, 120, 0, 16, 360.0 / 60.0 * second, 1, rgb565(255, 0, 0));
  gfx2d->drawThickTick(120, 120, 0, 110, 360.0 / 60.0 * second, 1, rgb565(255, 0, 0));
#endif
}

#ifdef GIF_BG
OswAppGifPlayer* bgGif = new OswAppGifPlayer();
#endif

void OswAppWatchface::setup(OswHal* hal) {
#ifdef GIF_BG
  bgGif->setup(hal);
#endif
}

void OswAppWatchface::loop(OswHal* hal) {
  if (hal->btn2Down()) {
    hal->decreaseBrightness(25);
    hal->clearBtn2();
  }
  if (hal->btn3Down()) {
    hal->increaseBrightness(25);
    hal->clearBtn3();
  }

#ifdef GIF_BG
  // if (millis() - 1000 > lastDraw) {
  bgGif->loop(hal);
  // lastDraw = millis();
  // }
#else
  hal->getCanvas()->getGraphics2D()->fill(rgb565(0, 0, 0));
#endif

  drawWatch(hal, hal->getCanvas()->getGraphics2D());
  hal->requestFlush();
}

void OswAppWatchface::stop(OswHal* hal) {
  // hal->disableDisplayBuffer();

#ifdef GIF_BG
  bgGif->stop(hal);
#endif
}
