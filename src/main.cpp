#include <Arduino.h>

#include "../lib/p3dt-lib-arduino/src/anim/p3dt_anim_doom_fire.h"
#include "../lib/p3dt-lib-arduino/src/gfx/p3dt_gfx_2d.h"
#include "../lib/p3dt-lib-arduino/src/gfx/p3dt_gfx_util.h"
#include "../lib/p3dt-lib-arduino/src/math/p3dt_math_angles.h"
#include "../lib/p3dt-lib-arduino/src/math/perlin/SimplexNoise.h"
#include "Arduino_Display.h"
#include "Arduino_ESP32SPI.h"
#include "Arduino_GFX.h"
#include "esp_task_wdt.h"
// pin mapping
#define TFT_CS 5
#define TFT_DC 27
#define TFT_RST 33
#define TFT_SCK 18
#define TFT_MOSI 23
#define TFT_MISO -1  // no data coming back

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO, VSPI /* spi_num */);
Arduino_GC9A01 *gfx = new Arduino_GC9A01(bus, TFT_RST, 0 /* rotation */, true /* IPS */);

#define BUF_W 240
#define BUF_H 240
#define CHUNK_H 16  // = 15 vertical chunks

SimplexNoise sn;

Graphics2D gfx2d(BUF_W, BUF_H, CHUNK_H /*chunk size*/);

uint8_t tipX = 0;
uint8_t tipY = 0;

void setup() {
  Serial.begin(115200);
  gfx->begin();
}
int16_t mix(uint8_t base, float n1, float n2) { return (n1 - abs(n2)) * base; }

void fillPerlin(uint16_t counter) {
  for (uint8_t x = 0; x < BUF_W; x++) {
    for (uint8_t y = 0; y < BUF_H; y++) {
      float n1 = sn.fractal(7, (x + counter) / 120.0, y / 120.0);  // sn.noise returns [-1,1]
      float n2 = sn.noise((x + counter) / 2, y / 2);               // sn.noise returns [-1,1]

      uint8_t r = n1 > 0 ? (n1 - n2) * 255 : 0;
      uint8_t g = r;
      uint8_t b = r;

      uint16_t color = rgb565(r, g, b);

      if (n1 < -.1) {  // deep water
        color = rgb565(72 * (1 + n1) + mix(5, n1, n2), 72 * (1 + n1) + mix(5, n1, n2), 190 * (1 + n1) + mix(5, n1, n2));
      } else if (n1 < 0) {  // shallow water plateaus
        color = rgb565(72 + mix(5, n1, n2), 72 + mix(5, n1, n2), 190 + mix(5, n1, n2));
      } else if (n1 < .0125) {  // beaches
        color = rgb565(200 + mix(5, n1, n2), 200 + mix(20, n1, n2), 47 + mix(5, n1, n2));
      } else if (n1 < .2) {  // meadows
        color = rgb565(85 + mix(40, n1, n2), 107 + mix(20, n1, n2), 47 + mix(20, n1, n2));
      } else if (n1 < .7) {  // forest
        color = rgb565((85 * (1 - (n1 - .2) * 2)) + (35 * ((n1 - .2) * 2)) - mix(10, n1, n2),
                       (107 * (1 - (n1 - .2) * 2)) + (57 * ((n1 - .2) * 2)) - mix(20, n1, n2),
                       (47 * (1 - (n1 - .2) * 2)) + (7 * ((n1 - .2) * 2)) - mix(10, n1, n2));
      } else {
        color = rgb565(35, 57, 7) - rgb565(85 + mix(40, n1, n2), 107 + mix(20, n1, n2), 47 + mix(20, n1, n2));
      }

      gfx2d.drawPixel(x, y, color);
    }
  }
}

void movePerlin() {}

void loop() {
  static bool drawOnce = true;
  static long seconds = 0;
  static long lastTick = 0;
  seconds = millis() / 1000;

  if (drawOnce) {
    drawOnce = false;
  }

  uint8_t cx = 119;
  uint8_t cy = 119;
  gfx2d.fill(rgb565(0, 0, 0));

  static uint16_t counter = 0;
  counter++;

  fillPerlin(counter);
  gfx2d.drawWatchFace(BUF_W / 2, BUF_H / 2, 119, rgb565(255, 255, 255));
  // draw handles
  // hour
  gfx2d.drawLine(cx, cy, rpx(cx, 33, h2d(seconds)), rpy(cy, 33, h2d(seconds)), WHITE);
  // // minute
  gfx2d.drawLine(cx, cy, rpx(cx, 66, m2d(seconds)), rpy(cy, 66, m2d(seconds)), WHITE);
  // // second
  gfx2d.drawLine(cx, cy, rpx(cx, 15, s2d(seconds) + 180), rpy(cy, 15, s2d(seconds) + 180), RED);  // short backwards
  tipX = rpx(cx, 90, s2d(seconds));
  tipY = rpy(cy, 90, s2d(seconds));
  gfx2d.drawLine(cx, cy, tipX, tipY, RED);  // long front
  // gfx->fillCircle(cx, cy, 4, RED);

  for (uint8_t chunk = 0; chunk < gfx2d.numChunks(); chunk++) {
    gfx->draw16bitRGBBitmap(0, chunk * CHUNK_H, gfx2d.getChunk(chunk), BUF_W, CHUNK_H);
  }

  Serial.println(1000 / (millis() - lastTick));  // fps
  lastTick = millis();
}