#include <Arduino.h>

// Libraries for display
#include "Arduino_Display.h"
#include "Arduino_ESP32SPI.h"
#include "Arduino_GFX.h"
// Libraries for SD card
#include "FS.h"
#include "SD.h"
#include "SPI.h"
// PNG Library
#include "pngle.h"
// Graphics Stuff
#include "../lib/arduino-library/src/gfx/p3dt_gfx_2d.h"
#include "../lib/arduino-library/src/gfx/p3dt_gfx_util.h"
#include "../lib/arduino-library/src/math/osm.h"
#include "../lib/arduino-library/src/math/p3dt_math_angles.h"

// pin mapping
#define TFT_CS 5
#define TFT_DC 27
#define TFT_RST 33
#define TFT_SCK 18
#define TFT_MOSI 23
#define TFT_MISO -1  // no data coming back

#define SD_CS 21
// SD_MISO 19
// for SCK, MOSI see TFT

#define MIN_LAYER 6
#define MAX_LAYER 6

float lat = 50.813;
float lon = 4.460;

String fileContents;

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO, VSPI /* spi_num */);
Arduino_GC9A01 *gfx = new Arduino_GC9A01(bus, TFT_RST, 0 /* rotation */, true /* IPS */);

#define TILE_H 255
#define TILE_W 255

// manual full buffering
#define BUF_W 240
#define BUF_H 240
#define CHUNK_H 4
Graphics2D screenBuffer(BUF_W, BUF_H, CHUNK_H);  // 240/CHUNK_H = 15 chunks
Graphics2D *tileBuffer;

long tileRender;

int16_t offsetX = 0;
int16_t offsetY = 0;

void setDrawOffset(float tileX, float tileY, int32_t cx, int32_t cy) {
  offsetX = cx - locOffset(tileX);
  offsetY = cy - locOffset(tileY);
}

void on_draw(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4]) {
  uint8_t r = rgba[0];  // 0 - 255
  uint8_t g = rgba[1];  // 0 - 255
  uint8_t b = rgba[2];  // 0 - 255
  uint8_t a = rgba[3];  // 0: fully transparent, 255: fully opaque
  if (a > 0 && x < 240 && y < 240) {
    screenBuffer.drawPixel(x + offsetX, y + offsetY, rgb565(r, g, b));
  }
}

void drawTile(int8_t z, float tilex, float tiley, int32_t offsetx, int32_t offsety) {
  long start = millis();
  String tilePath = String("/maps/") + String(z) + "/" + String((int32_t)tilex) + "/" + String((int32_t)tiley) + ".png";
  File file = SD.open(tilePath);
  // uint32_t fileSize = file.size();
  pngle_t *pngle = pngle_new();

  setDrawOffset(tilex, tiley, offsetx, offsety);

  pngle_set_draw_callback(pngle, on_draw);

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
    if (remain > 0) memmove(buf, buf + fed, remain);
  }

  pngle_destroy(pngle);

  file.close();
  tileRender = millis() - start;
}

void setup() {
  tileBuffer = new Graphics2D(128, 128, 4);  // 240/CHUNK_H = 15 chunks
  Serial.begin(115200);
  gfx->begin();

  // Initialize SD card
  SD.begin(SD_CS);
  if (!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;  // init failed
  }
}

void loop() {
  static long seconds = 0;
  static int8_t zoom = MIN_LAYER;
  static bool zoomIn = true;

  seconds++;

  lon += 0.2;
  if (lon >= 180) {
    lon = -180;
  }
  lat += 0.2;
  if (lat >= 80) {
    lat = -80;
  }

  // uint8_t cx = 119;
  // uint8_t cy = 119;

  float tilex = lon2tilex(lon, zoom);
  float tiley = lat2tiley(lat, zoom);
  screenBuffer.fill(0);
  drawTile(zoom, tilex, tiley, 119, 119);
  // TODO below is not optimal, we have cases where nothing needs to be drawn
  if (locOffset(tilex) < .5 && locOffset(tiley) < .5) {
    // top left
    drawTile(zoom, tilex - 1, tiley, 119 - TILE_W, 119);
    drawTile(zoom, tilex - 1, tiley - 1, 119 - TILE_W, 119 - TILE_H);
    drawTile(zoom, tilex, tiley - 1, 119, 119 - TILE_H);
  } else if (locOffset(tilex) < .5 && locOffset(tiley) >= .5) {
    // bot left
    drawTile(zoom, tilex - 1, tiley, 119 - TILE_W, 119);
    drawTile(zoom, tilex - 1, tiley + 1, 119 - TILE_W, 119 + TILE_H);
    drawTile(zoom, tilex, tiley + 1, 119, 119 + TILE_H);
  } else if (locOffset(tilex) >= .5 && locOffset(tiley) >= .5) {
    // bot right
    drawTile(zoom, tilex, tiley + 1, 119, 119 + TILE_H);
    drawTile(zoom, tilex + 1, tiley + 1, 119 + TILE_W, 119 + TILE_H);
    drawTile(zoom, tilex + 1, tiley, 119 + TILE_W, 119);
  } else {
    // top right
    drawTile(zoom, tilex + 1, tiley, 119 + TILE_W, 119);
    drawTile(zoom, tilex + 1, tiley - 1, 119 + TILE_W, 119 - TILE_H);
    drawTile(zoom, tilex, tiley - 1, 119, 119 - TILE_H);
  }

  screenBuffer.drawCircle(119, 119, 4, rgb565(255, 0, 0));

  Serial.println(tileRender);

  // gfx->fillScreen(BLACK);
  for (uint8_t chunk = 0; chunk < screenBuffer.numChunks(); chunk++) {
    gfx->draw16bitRGBBitmap(0, chunk * CHUNK_H, screenBuffer.getChunk(chunk), BUF_W, CHUNK_H);
  }

  // gfx->setTextColor(RED);
  // gfx->setCursor(12, 120);
  // gfx->print(tileRender);

  // gfx->drawLine(cx, cy, rpx(cx, 33, h2d(seconds)), rpy(cy, 33, h2d(seconds)), BLUE);
  // // // minute
  // gfx->drawLine(cx, cy, rpx(cx, 66, m2d(seconds)), rpy(cy, 66, m2d(seconds)), BLUE);

  // Serial.print("Free memory: ");
  // Serial.println(xPortGetFreeHeapSize());

  // bounce the zoom
  if (zoomIn) {
    zoom++;
    if (zoom > MAX_LAYER) {
      zoomIn = false;
      zoom = MAX_LAYER;
    }
  } else {
    zoom--;
    if (zoom < MIN_LAYER) {
      zoomIn = true;
      zoom = MIN_LAYER;
    }
  }
}