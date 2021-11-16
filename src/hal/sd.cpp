#define FS_NO_GLOBALS
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <gfx_2d.h>
#include <gfx_util.h>
#include <math_osm.h>
#include <pngle.h>

#include "osw_hal.h"
#include "osw_pins.h"

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)

// this is a nasty hack and depends on hal/esp32/sd_filesystem.cpp
extern bool _hasSD;
extern bool _isSDMounted;

bool OswHal::hasSD(void) { return _hasSD; }
bool OswHal::isSDMounted(void) { return _isSDMounted; }

uint64_t OswHal::sdCardSize(void) { return SD.cardSize(); }
// ugly trickery:
Graphics2D *pngBuffer;
int16_t pngOffsetX = 0;
int16_t pngOffsetY = 0;
int16_t alphaPlaceHolder = 0;

void setDrawOffset(float tileX, float tileY, int32_t centerX, int32_t centerY) {
  pngOffsetX = centerX - tileOffset(tileX);
  pngOffsetY = centerY - tileOffset(tileY);
}

void drawPng(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4]) {
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
    pngBuffer->drawPixel(x + pngOffsetX, y + pngOffsetY, rgb565(r, g, b));
  } else if (a == 0) {
    pngBuffer->drawPixel(x + pngOffsetX, y + pngOffsetY, alphaPlaceHolder);
  }
}

void loadPNGHelper(Graphics2D *target, const char *path) {
  File file = SD.open(path);
  // uint32_t fileSize = file.size();
  pngle_t *pngle = pngle_new();

  pngle_set_draw_callback(pngle, drawPng);

  // Feed data to pngle
  uint8_t buf[1024];
  int remain = 0;
  int len;

  while ((len = file.read(buf, sizeof(buf) - remain)) > 0) {
    int fed = pngle_feed(pngle, buf, remain + len);

    if (fed < 0) {
      // Uninitialized
      Serial.println(pngle_error(pngle));
    }

    remain = remain + len - fed;
    if (remain > 0) {
      memmove(buf, buf + fed, remain);
    }
  }

  pngle_destroy(pngle);

  file.close();
}

void OswHal::setPNGAlphaPlaceHolder(uint16_t color) { alphaPlaceHolder = color; }

void OswHal::loadPNGfromSD(Graphics2D *target, const char *path) {
  // Serial.print("Loading ");
  // Serial.println(path);

  pngBuffer = target;
  // the setDrawOffset is a dirty hack
  pngOffsetX = 0;
  pngOffsetY = 0;
  loadPNGHelper(target, path);
}
void OswHal::loadOsmTile(Graphics2D *target, int8_t z, float tileX, float tileY, int32_t offsetX, int32_t offsetY) {
  pngBuffer = target;
  // Serial.println("loadOsmTile");

  if (offsetX <= -256 || offsetY <= -256 || offsetX >= target->getWidth() || offsetY >= target->getHeight()) {
    // skip if tile is not visible
    return;
  }

  String tilePath = String("/map/") + String(z) + "/" + String((int32_t)tileX) + "/" + String((int32_t)tileY) + ".png";
  pngOffsetX = offsetX;
  pngOffsetY = offsetY;

  loadPNGHelper(target, tilePath.c_str());
  // debug helper to see tile boundaries:
  // target->drawFrame(offsetX, offsetY, 256, 256, rgb565(200, 0, 0));
}

void OswHal::sdOff(void) { SD.end(); }

#endif
