#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <gfx_2d.h>
#include <gfx_util.h>
#include <math_osm.h>
#include <pngle.h>

#include "osw_hal.h"
#include "osw_pins.h"

uint8_t OswHal::setupSD() {
  SD.begin(SD_CS);

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    _hasSD = false;
    return ERR_SD_MISSING;
  } else {
    _hasSD = true;
    if (!SD.begin(SD_CS)) {
      return ERR_SD_MOUNT_FAILED;
    }
    _isSDMounted = true;
  }

  return 0;
}

bool OswHal::hasSD(void) { return _hasSD; }
bool OswHal::isSDMounted(void) { return _isSDMounted; }

uint64_t OswHal::sdCardSize(void) { return SD.cardSize(); }
// ugly trickery:
Graphics2D *pngBuffer;
int16_t pngOffsetX = 0;
int16_t pngOffsetY = 0;
int16_t alphaPlaceHolder = 0;

void setDrawOffset(float tileX, float tileY, int32_t cx, int32_t cy) {
  pngOffsetX = cx - tileOffset(tileX);
  pngOffsetY = cy - tileOffset(tileY);
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
  Serial.println("open");
  pngle_t *pngle = pngle_new();
  Serial.println("new");

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
  Serial.println("done");
}

void OswHal::setPNGAlphaPlaceHolder(uint16_t color) { alphaPlaceHolder = color; }

void OswHal::loadPNG(Graphics2D *target, const char *path) {
  Serial.print("Loading ");
  Serial.println(path);

  pngBuffer = target;
  // the setDrawOffset is a dirty hack
  pngOffsetX = 0;
  pngOffsetY = 0;
  loadPNGHelper(target, path);
}
void OswHal::loadOsmTile(Graphics2D *target, int8_t z, float tilex, float tiley, int32_t offsetx, int32_t offsety) {
  pngBuffer = target;

  String tilePath = String("/maps/") + String(z) + "/" + String((int32_t)tilex) + "/" + String((int32_t)tiley) + ".png";
  setDrawOffset(tilex, tiley, offsetx, offsety);

  loadPNGHelper(target, tilePath.c_str());
}
