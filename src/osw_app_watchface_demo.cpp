
#include "osw_app_watchface_demo.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

#define IMG_W 24
#define IMG_H 240

// this is dirty:
Graphics2D* handleMinute;
Graphics2D* handleHour;
Graphics2D* amongUsSprites;
Graphics2D* amongUsSprite;
Graphics2D* gfx;

void OswAppWatchFaceDemo::setup(OswHal* hal) {
  gfx = hal->getCanvas()->getGraphics2D();

  handleMinute = new Graphics2D(IMG_W, IMG_H, 8);
  handleHour = new Graphics2D(IMG_W, IMG_H, 8);
  amongUsSprites = new Graphics2D(16 * 12, 22, 22);
  uint16_t alphaPlaceholder = rgb565(255, 255, 255);
  amongUsSprite = new Graphics2D(15, 22, 22);

  hal->setPNGAlphaPlaceHolder(alphaPlaceholder);
  hal->loadPNG(amongUsSprites, "/sprite_amongus_16x22s12.png");
  //
  // hal->loadPNG(handleMinute, "/watchface01m.png");
  // hal->loadPNG(handleHour, "/watchface01h.png");
  gfx->enableMask(alphaPlaceholder);
  Serial.println("loading done");
}

void OswAppWatchFaceDemo::loop(OswHal* hal) {
  static uint16_t counter = 0;

  counter++;

  // draw using a source offset (second two parameters)

  for (uint8_t x = 0; x < IMG_W; x++) {
    for (uint8_t y = 0; y < IMG_H; y++) {
      handleMinute->drawPixel(x, y, rgb565((x + counter) % 256, (y + counter + 128) % 255, 10));
    }
  }

  amongUsSprite->fillFrame(0, 0, 16, 22, rgb565(255, 255, 255));

  amongUsSprite->drawGraphics2D(0, 0, amongUsSprites, (15 * (counter % 12)), 0, 15, 22);

  // hal->getCanvas()->getGraphics2D()->fillFrame(0, 0, 240, 240, rgb565(10, 10, 10));
  // hal->getCanvas()->getGraphics2D()->drawGraphics2D_rotated(handleHour, 120, 120, 12, 120, -(counter / 5.0) / 60);
  // gfx->drawGraphics2D_rotated(120, 120, handleMinute, IMG_W / 2, IMG_H / 2, -counter / (10 * PI));

  // gfx->fillFrame(0, 0, 240, 240, rgb565(0, 0, 0));
  gfx->enableAlpha(0.75);
  gfx->drawGraphics2D_rotated(120 + rotateX(72, 72, 0, 0, (counter - 45) / 50.0),  //
                              120 + rotateY(72, 72, 0, 0, (counter - 45) / 50.0),  //
                              amongUsSprite, 8, 11, counter / 50.0);
  gfx->disableAplha();
  // gfx->drawGraphics2D_2x(15, 120 - 22, amongUsSprite);
  // gfx->drawGraphics2D(10, 142, amongUsSprites);
}
