#include <Arduino.h>

#include "Arduino_Display.h"  // Various display driver
#include "Arduino_ESP32SPI.h"
#include "Arduino_GFX.h"  // Core graphics library

#define TFT_CS 5
// #define TFT_CS -1 // for display without CS pin
// #define TFT_DC 16
#define TFT_DC 27
// #define TFT_DC -1 // for display without DC pin (9-bit SPI)
// #define TFT_RST 17
#define TFT_RST 33

// Arduino_DataBus *bus = new Arduino_HWSPI(TFT_DC, TFT_CS);
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, 18 /* SCK */, 23 /* MOSI */, -1 /* MISO */, VSPI /* spi_num */);

Arduino_GC9A01 *gfx = new Arduino_GC9A01(bus, TFT_RST, 0 /* rotation */, true /* IPS */);

void setup() {
  Serial.begin(115200);
  gfx->begin();
  gfx->fillScreen(GREEN);
}

uint x;

void loop() {
  Serial.println("Hello World");
  delay(1000);
  gfx->drawLine(0, 0, 239, 239, RED);
}