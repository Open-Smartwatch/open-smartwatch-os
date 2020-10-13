#include <Arduino.h>
#include <osw_hal.h>
#include <osw_pins.h>


#define MIN_LAYER 0
#define MAX_LAYER 6

OswHal hal;

String fileContents;

#define TILE_H 255
#define TILE_W 255

// manual full buffering
#define BUF_W 240
#define BUF_H 240
#define CHUNK_H 4
Graphics2D screenBuffer(BUF_W, BUF_H, CHUNK_H);  // 240/CHUNK_H = 15 chunks
Graphics2D *buffer;

enum Mode {
  BRIGHTNESS = 0,
  ZOOM = 1
  // VIEW = 2
};
#define NUM_MODES 2
uint8_t mode = BRIGHTNESS;

void setup() {
  buffer = new Graphics2D(128, 128, 4);  // 240/CHUNK_H = 15 chunks

  Serial.begin(115200);

  hal.setupPower();
  hal.setupButtons();
  hal.setupSensors();
  hal.setupDisplay();
  hal.setupGps();
}

void loop() {
  static long seconds = 0;
  seconds++;
  static int8_t zoom = MAX_LAYER;

  // lat += 0.2;
  // if (lat >= 80) {
  //   lat = -80;
  // }

  // uint8_t cx = 119;
  // uint8_t cy = 119;

  // float tilex = lon2tilex(lon, zoom);
  // float tiley = lat2tiley(lat, zoom);
  // Serial.print("tilex: ");
  // Serial.print(tilex);
  // Serial.print(", tileOffset(tilex): ");
  // Serial.print(tileOffset(tilex));
  // Serial.print("tiley: ");
  // Serial.print(tiley);
  // Serial.print(", tileOffset(tiley): ");
  // Serial.println(tileOffset(tiley));

  screenBuffer.fill(0);
  // drawTile(zoom, tilex, tiley, 119, 119);
  // // TODO below is not optimal, we have cases where nothing needs to be drawn
  // if (tileOffset(tilex) < 128 && tileOffset(tiley) < 128) {
  //   // top left
  //   Serial.println("top left");
  //   drawTile(zoom, tilex - 1, tiley, 119 - TILE_W, 119);
  //   drawTile(zoom, tilex - 1, tiley - 1, 119 - TILE_W, 119 - TILE_H);
  //   drawTile(zoom, tilex, tiley - 1, 119, 119 - TILE_H);
  // } else if (tileOffset(tilex) < 128 && tileOffset(tiley) >= 128) {
  //   // bot left
  //   Serial.println("bot left");
  //   drawTile(zoom, tilex - 1, tiley, 119 - TILE_W, 119);
  //   drawTile(zoom, tilex - 1, tiley + 1, 119 - TILE_W, 119 + TILE_H);
  //   drawTile(zoom, tilex, tiley + 1, 119, 119 + TILE_H);
  // } else if (tileOffset(tilex) >= 128 && tileOffset(tiley) >= 128) {
  //   // bot right
  //   Serial.println("bot right");
  //   drawTile(zoom, tilex, tiley + 1, 119, 119 + TILE_H);
  //   drawTile(zoom, tilex + 1, tiley + 1, 119 + TILE_W, 119 + TILE_H);
  //   drawTile(zoom, tilex + 1, tiley, 119 + TILE_W, 119);
  // } else {
  //   // top right
  //   Serial.println("top right");
  //   drawTile(zoom, tilex + 1, tiley, 119 + TILE_W, 119);
  //   drawTile(zoom, tilex + 1, tiley - 1, 119 + TILE_W, 119 - TILE_H);
  //   drawTile(zoom, tilex, tiley - 1, 119, 119 - TILE_H);
  // }

  // screenBuffer.drawCircle(119, 119, 4, rgb565(255, 0, 0));

  // Serial.println(tileRender);

  // gfx->fillScreen(BLACK);
  // for (uint8_t chunk = 0; chunk < screenBuffer.numChunks(); chunk++) {
  //   gfx->draw16bitRGBBitmap(0, chunk * CHUNK_H, screenBuffer.getChunk(chunk), BUF_W, CHUNK_H);
  // }

  // if (gps.satellites.isValid()) {
  //   gfx->setTextColor(GREEN);
  // } else {
  //   gfx->setTextColor(RED);
  // }
  // gfx->setCursor(12, 120);
  // gfx->print(gps.satellites.value());

  // gfx->drawLine(cx, cy, rpx(cx, 33, h2d(seconds)), rpy(cy, 33, h2d(seconds)), BLUE);
  // // // minute
  // gfx->drawLine(cx, cy, rpx(cx, 66, m2d(seconds)), rpy(cy, 66, m2d(seconds)), BLUE);

  // Serial.print("Free memory: ");
  // Serial.println(xPortGetFreeHeapSize());

  // lat = gps.location.lat();
  // lon = gps.location.lng();

  // lat = (uint8_t)lat ? 1 : lat;
  // lon = (uint8_t)lon ? 1 : lon;

  // Serial.print("ALT=");
  // Serial.println(gps.altitude.isValid());
  // Serial.print("SAT=");
  // Serial.println(gps.satellites.isValid());
  // Serial.print("value=");
  // Serial.println(gps.satellites.value());
  // Serial.print("#################### lat=");
  // Serial.print(gps.location.lat());
  // Serial.print("   long=");
  // Serial.println(gps.location.lng());

  // Serial.print("Temperature = ");
  // Serial.print(bme.readTemperature());
  // Serial.println(" *C");

  // Serial.print("Pressure = ");

  // Serial.print(bme.readPressure() / 100.0F);
  // Serial.println(" hPa");

  // Serial.print("STAT_PWR: ");
  // Serial.println(digitalRead(STAT_PWR));

  if (digitalRead(BTN_1) == LOW) {
    delay(1000);  // FIXME
    mode++;
    mode = mode >= NUM_MODES ? 0 : mode;
    Serial.print("MODE: ");
    Serial.println(mode);
  }
  if (digitalRead(BTN_2) == HIGH) {
    delay(250);  // FIXME
    switch (mode) {
      case BRIGHTNESS:
        Serial.println("--> Brightness +");

        hal.increaseBrightness(50);
        break;
      case ZOOM:
        Serial.println("--> Zoom +");

        if (zoom < MAX_LAYER) {
          zoom += 1;
        } else {
          zoom = MAX_LAYER;
        }
        break;
    }
  }
  if (digitalRead(BTN_3) == HIGH) {
    delay(250);  // FIXME
    switch (mode) {
      case BRIGHTNESS:
        Serial.println("--> Brightness -");
        hal.decreaseBrightness(50);
        break;
      case ZOOM:
        Serial.println("--> Zoom -");
        if (zoom > MIN_LAYER) {
          zoom -= 1;
        } else {
          zoom = MIN_LAYER;
        }
        break;
    }
  }

  // Serial.print("BTN_1: ");
  // Serial.println(digitalRead(BTN_1));
  // Serial.print("BTN_2: ");
  // Serial.println(digitalRead(BTN_2));
  // Serial.print("BTN_3: ");
  // Serial.println(digitalRead(BTN_3));

  // Serial.print("B_MON: ");
  // Serial.println(analogRead(B_MON));
}