#include <Arduino.h>

#include "../lib/p3dt-lib-arduino/anim/p3dt_anim_doom_fire.h"
#include "../lib/p3dt-lib-arduino/gfx/p3dt_gfx_2d.h"
#include "../lib/p3dt-lib-arduino/gfx/p3dt_gfx_util.h"
#include "../lib/p3dt-lib-arduino/math/p3dt_gfx_angles.h";
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
#define FIRE_WIDTH 240
#define FIRE_HEIGHT 80
uint8_t **firePixels = new uint8_t *[FIRE_HEIGHT];

Graphics2D gfx2d(BUF_W, BUF_H, CHUNK_H /*chunk size*/);

uint8_t tipX = 0;
uint8_t tipY = 0;

TaskHandle_t Task1;
void fireTask(void *pvParameters) {
  while (true) {
    calcFire(firePixels, BUF_W, FIRE_HEIGHT);
    uint8_t fireOffset = (BUF_W - FIRE_HEIGHT);
    if (tipY > fireOffset) {
      firePixels[tipY - fireOffset][tipX - 1] = 30;
      firePixels[tipY - fireOffset][tipX] = 34;
      firePixels[tipY - fireOffset + 1][tipX] = 30;
      firePixels[tipY - fireOffset][tipX + 1] = 30;
    }
    delay(10);
  }
  // doCalcFire = false;
}

void setup() {
  Serial.begin(115200);
  gfx->begin();
  // setup array rows
  for (int i = 0; i < FIRE_HEIGHT; i++) {
    firePixels[i] = new uint8_t[FIRE_WIDTH];
  }
  setupFire(firePixels, FIRE_WIDTH, FIRE_HEIGHT);
  xTaskCreatePinnedToCore(fireTask, "FireTask", 1000 /*stack*/, NULL /*input*/, 0 /*prio*/, &Task1 /*handle*/, 0);
}

void loop() {
  static bool drawOnce = true;
  static long seconds = 0;
  static long lastTick = 0;
  seconds++;

  if (drawOnce) {
    drawOnce = false;
    gfx->fillRect(0, 0, 240, 240, BLACK);
    Serial.print("Setup running on core ");
    Serial.println(xPortGetCoreID());
  }

  uint8_t cx = 119;
  uint8_t cy = 119;
  gfx2d.fill(rgb565(0, 0, 0));

  mapFire(firePixels, FIRE_WIDTH, FIRE_HEIGHT, &gfx2d, 0, (BUF_H - FIRE_HEIGHT));
  gfx2d.drawWatchFace(BUF_W / 2, BUF_H / 2, 119, rgb565(255, 255, 255));
  // draw handles
  // hour
  gfx2d.drawLine(cx, cy, rpx(cx, 33, h2d(seconds)), rpy(cy, 33, h2d(seconds)), WHITE);
  // // minute
  gfx2d.drawLine(cx, cy, rpx(cx, 66, m2d(seconds)), rpy(cy, 66, m2d(seconds)), BLUE);
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