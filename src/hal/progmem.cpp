#include <Arduino.h>
#include <pngle.h>

#include "osw_hal.h"

// ugly trickery:
Graphics2D* pngBufferProgmem;
int16_t pngOffsetXProgmem = 0;
int16_t pngOffsetYProgmem = 0;
int16_t alphaPlaceHolderProgmem = 0;

void drawPngProgmem(pngle_t* pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4]) {
  uint8_t r = rgba[0];  // 0 - 255
  uint8_t g = rgba[1];  // 0 - 255
  uint8_t b = rgba[2];  // 0 - 255
  uint8_t a = rgba[3];  // 0: fully transparent, 255: fully opaque

  // Serial.print(r);
  // Serial.print(",");
  // Serial.print(g);
  // Serial.print(",");
  // Serial.println(b);

  if (a > 0) {
    pngBufferProgmem->drawPixel(x + pngOffsetXProgmem, y + pngOffsetYProgmem, rgb565(r, g, b));
  } else if (a == 0) {
    pngBufferProgmem->drawPixel(x + pngOffsetXProgmem, y + pngOffsetYProgmem, alphaPlaceHolderProgmem);
  }
}

void OswHal::loadPNGfromProgmem(Graphics2D* target, const unsigned char* data, unsigned int length) {
  //

  pngBufferProgmem = target;
  pngOffsetXProgmem = 0;
  pngOffsetYProgmem = 0;

  pngle_t* pngle = pngle_new();

  pngle_set_draw_callback(pngle, drawPngProgmem);
  int fed = pngle_feed(pngle, data, length);
  if (fed < 0) {
    // Uninitialized
    Serial.println(pngle_error(pngle));
  }
  pngle_destroy(pngle);
}
