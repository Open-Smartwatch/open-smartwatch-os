#include <Arduino.h>

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

void setup() {
  Serial.begin(115200);
  gfx->begin();
}

void loop() {
  gfx->fillScreen(BLACK);
  gfx->setTextColor(WHITE);
  gfx->setCursor(64, 64);
  gfx->print("Hello World!");

  delay(1000);
}