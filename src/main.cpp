#include <Arduino.h>

// Libraries for display
#include "Arduino_Display.h"
#include "Arduino_ESP32SPI.h"
#include "Arduino_GFX.h"
// Libraries for SD card
#include "FS.h"
#include "HardwareSerial.h"
#include "SD.h"
#include "SPI.h"
#include "Wire.h"
// Graphics
#include "../lib/arduino-library/src/gfx/p3dt_gfx_2d.h"
#include "../lib/arduino-library/src/gfx/p3dt_gfx_util.h"
#include "../lib/arduino-library/src/math/osm.h"
#include "../lib/arduino-library/src/math/p3dt_math_angles.h"
#include "pngle.h"
// GPS
#include "TinyGPS++.h"
// Sensors
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

// pin mapping
#define TFT_CS 5
#define TFT_DC 12
#define TFT_RST 33
#define TFT_SCK 18
#define TFT_MOSI 23
#define TFT_MISO -1  // no data coming back
#define TFT_LED 9

#define SD_CS 4
// SD_MISO 19
// for SCK, MOSI see TFT

#define GPS_RX 14
#define GPS_TX 27
#define GPS_FON 26
#define RX1 27
#define TX1 14

#define RTC_INT 32

#define BTN_1 0
#define BTN_2 10
#define BTN_3 13

#define SCL 22
#define SDA 21

#define STAT_PWR 15
#define B_MON 25

#define MIN_LAYER 0
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

HardwareSerial SerialGPS(1);
TinyGPSPlus gps;

Adafruit_BME280 bme;

enum Mode {
  BRIGHTNESS = 0,
  ZOOM = 1
  // VIEW = 2
};
#define NUM_MODES 2
uint8_t mode = BRIGHTNESS;

uint8_t brightness = 255;

void setDrawOffset(float tileX, float tileY, int32_t cx, int32_t cy) {
  offsetX = cx - tileOffset(tileX);
  offsetY = cy - tileOffset(tileY);
  Serial.print("offsetX: ");
  Serial.print(offsetX);
  Serial.print(", offsetY: ");
  Serial.println(offsetY);
}

void on_draw(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4]) {
  uint8_t r = rgba[0];  // 0 - 255
  uint8_t g = rgba[1];  // 0 - 255
  uint8_t b = rgba[2];  // 0 - 255
  uint8_t a = rgba[3];  // 0: fully transparent, 255: fully opaque
  if (a > 0 && x < 255 && y < 255) {
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
  Wire.begin(SDA, SCL, 100000L);
  tileBuffer = new Graphics2D(128, 128, 4);  // 240/CHUNK_H = 15 chunks
  Serial.begin(115200);
  SerialGPS.begin(9600, SERIAL_8N1, RX1, TX1);

  pinMode(BTN_1, INPUT);
  pinMode(BTN_2, INPUT);
  pinMode(BTN_3, INPUT);
  pinMode(STAT_PWR, INPUT);
  pinMode(B_MON, INPUT);

  pinMode(GPS_FON, OUTPUT);
  digitalWrite(GPS_FON, HIGH);

  // pinMode(TFT_LED, OUTPUT);
  // digitalWrite(TFT_LED, 128);
  ledcAttachPin(TFT_LED, 1);
  ledcSetup(1, 12000, 8);  // 12 kHz PWM, 8-bit resolution

  uint8_t status = bme.begin(0x76, &Wire);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    // while (1) delay(10);
  }

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

  SerialGPS.print("$PMTK353,1,0,0,0,0*2A\r/n"); // Full On: GPS
  // SerialGPS.print("$PMTK353,1,1,1,0,0*2A\r\n"); // Full On: GPS, GLONASS, Galileo
}

void loop() {
  static long seconds = 0;
  static int8_t zoom = MAX_LAYER;

  while (SerialGPS.available() > 0) {
    gps.encode(SerialGPS.read());
    // Serial.write(SerialGPS.read());
  }

  seconds++;

  ledcWrite(1, brightness);
  // lat += 0.2;
  // if (lat >= 80) {
  //   lat = -80;
  // }

  // uint8_t cx = 119;
  // uint8_t cy = 119;

  float tilex = lon2tilex(lon, zoom);
  float tiley = lat2tiley(lat, zoom);
  Serial.print("tilex: ");
  Serial.print(tilex);
  Serial.print(", tileOffset(tilex): ");
  Serial.print(tileOffset(tilex));
  Serial.print("tiley: ");
  Serial.print(tiley);
  Serial.print(", tileOffset(tiley): ");
  Serial.println(tileOffset(tiley));

  screenBuffer.fill(0);
  drawTile(zoom, tilex, tiley, 119, 119);
  // TODO below is not optimal, we have cases where nothing needs to be drawn
  if (tileOffset(tilex) < 128 && tileOffset(tiley) < 128) {
    // top left
    Serial.println("top left");
    drawTile(zoom, tilex - 1, tiley, 119 - TILE_W, 119);
    drawTile(zoom, tilex - 1, tiley - 1, 119 - TILE_W, 119 - TILE_H);
    drawTile(zoom, tilex, tiley - 1, 119, 119 - TILE_H);
  } else if (tileOffset(tilex) < 128 && tileOffset(tiley) >= 128) {
    // bot left
    Serial.println("bot left");
    drawTile(zoom, tilex - 1, tiley, 119 - TILE_W, 119);
    drawTile(zoom, tilex - 1, tiley + 1, 119 - TILE_W, 119 + TILE_H);
    drawTile(zoom, tilex, tiley + 1, 119, 119 + TILE_H);
  } else if (tileOffset(tilex) >= 128 && tileOffset(tiley) >= 128) {
    // bot right
    Serial.println("bot right");
    drawTile(zoom, tilex, tiley + 1, 119, 119 + TILE_H);
    drawTile(zoom, tilex + 1, tiley + 1, 119 + TILE_W, 119 + TILE_H);
    drawTile(zoom, tilex + 1, tiley, 119 + TILE_W, 119);
  } else {
    // top right
    Serial.println("top right");
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

  if (gps.satellites.isValid()) {
    gfx->setTextColor(GREEN);
  } else {
    gfx->setTextColor(RED);
  }
  gfx->setCursor(12, 120);
  gfx->print(gps.satellites.value());

  // gfx->drawLine(cx, cy, rpx(cx, 33, h2d(seconds)), rpy(cy, 33, h2d(seconds)), BLUE);
  // // // minute
  // gfx->drawLine(cx, cy, rpx(cx, 66, m2d(seconds)), rpy(cy, 66, m2d(seconds)), BLUE);

  // Serial.print("Free memory: ");
  // Serial.println(xPortGetFreeHeapSize());

  lat = gps.location.lat();
  lon = gps.location.lng();

  lat = (uint8_t)lat ? 1 : lat;
  lon = (uint8_t)lon ? 1 : lon;

  Serial.print("ALT=");
  Serial.println(gps.altitude.isValid());
  Serial.print("SAT=");
  Serial.println(gps.satellites.isValid());
  Serial.print("value=");
  Serial.println(gps.satellites.value());
  Serial.print("#################### lat=");
  Serial.print(gps.location.lat());
  Serial.print("   long=");
  Serial.println(gps.location.lng());

  // Serial.print("Temperature = ");
  // Serial.print(bme.readTemperature());
  // Serial.println(" *C");

  // Serial.print("Pressure = ");

  // Serial.print(bme.readPressure() / 100.0F);
  // Serial.println(" hPa");

  Serial.print("STAT_PWR: ");
  Serial.println(digitalRead(STAT_PWR));

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

        if (brightness > 205) {
          brightness = 255;
        } else {
          brightness += 50;
        }
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

        if (brightness < 50) {
          brightness = 0;
        } else {
          brightness -= 50;
        }
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

  Serial.print("BTN_1: ");
  Serial.println(digitalRead(BTN_1));
  Serial.print("BTN_2: ");
  Serial.println(digitalRead(BTN_2));
  Serial.print("BTN_3: ");
  Serial.println(digitalRead(BTN_3));

  Serial.print("B_MON: ");
  Serial.println(analogRead(B_MON));
}