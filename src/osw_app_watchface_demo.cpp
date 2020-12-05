
#include "osw_app_watchface_demo.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

#define IMG_W 24
#define IMG_H 240

Graphics2D* handleMinute;
Graphics2D* handleHour;

void OswAppWatchFaceDemo::run(OswHal* hal) {
  static bool setup = true;
  static uint16_t counter = 0;
  counter++;

  if (setup) {
    handleMinute = new Graphics2D(IMG_W, IMG_H, 8);
    handleHour = new Graphics2D(IMG_W, IMG_H, 8);
    // hal->loadPNG(handleMinute, "/watchface01m.png");
    // hal->loadPNG(handleHour, "/watchface01h.png");
    hal->getCanvas()->getGraphics2D()->setMaskEnabled(false);                  // default (0,0,0)
    hal->getCanvas()->getGraphics2D()->setMaskEnabled(rgb565(255, 255, 255));  // default (0,0,0)

    setup = false;
  }

  for (uint8_t x = 0; x < IMG_W; x++) {
    for (uint8_t y = 0; y < IMG_H; y++) {
      handleMinute->drawPixel(x, y, rgb565((x + counter) % 256, (y + counter + 128) % 255, 10));
    }
  }

  // hal->getCanvas()->getGraphics2D()->fillFrame(0, 0, 240, 240, rgb565(10, 10, 10));
  // hal->getCanvas()->getGraphics2D()->drawGraphics2D_rotated(handleHour, 120, 120, 12, 120, -(counter / 5.0) / 60);
  hal->getCanvas()->getGraphics2D()->drawGraphics2D_rotated(120, 120, handleMinute, IMG_W / 2, IMG_H / 2,
                                                            -counter / (10 * PI));
}
