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
    Serial.println("No SD card attached");
    return ERR_SD_MISSING;
  } else {
    Serial.println("Card Mount Failed");
    return ERR_SD_MOUNT_FAILED;
  }

  Serial.println("Initialized SD card...");
  return 0;
}

Graphics2D *tileBuffer;
int16_t pngOffsetX = 0;
int16_t pngOffsetY = 0;

void setDrawOffset(float tileX, float tileY, int32_t cx, int32_t cy) {
  pngOffsetX = cx - tileOffset(tileX);
  pngOffsetY = cy - tileOffset(tileY);
}

void drawPng(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4]) {
  uint8_t r = rgba[0];  // 0 - 255
  uint8_t g = rgba[1];  // 0 - 255
  uint8_t b = rgba[2];  // 0 - 255
  uint8_t a = rgba[3];  // 0: fully transparent, 255: fully opaque
  if (a > 0 && x < 255 && y < 255) {
    tileBuffer->drawPixel(x + pngOffsetX, y + pngOffsetY, rgb565(r, g, b));
  }
}

void OswHal::loadOsmTile(Graphics2D *target, int8_t z, float tilex, float tiley, int32_t offsetx, int32_t offsety) {
  tileBuffer = target;

  String tilePath = String("/maps/") + String(z) + "/" + String((int32_t)tilex) + "/" + String((int32_t)tiley) + ".png";
  File file = SD.open(tilePath);
  // uint32_t fileSize = file.size();
  pngle_t *pngle = pngle_new();

  setDrawOffset(tilex, tiley, offsetx, offsety);

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
