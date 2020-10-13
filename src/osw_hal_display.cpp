#include <Arduino_Display.h>
#include <Arduino_ESP32SPI.h>
#include <Arduino_GFX.h>
#include <gfx_2d.h>
#include <gfx_util.h>
#include <math_osm.h>
#include <pngle.h>

#include "osw_hal.h"
#include "osw_pins.h"

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO, VSPI /* spi_num */);
Arduino_GC9A01 *gfx = new Arduino_GC9A01(bus, TFT_RST, 0 /* rotation */, true /* IPS */);

void OswHal::setupDisplay(void) {
#ifdef ESP32
  ledcAttachPin(TFT_LED, 1);
  ledcSetup(1, 12000, 8);  // 12 kHz PWM, 8-bit resolution
#else
  pinMode(TFT_LED, OUTPUT);
#endif
  gfx->begin();
}

void OswHal::setBrightness(uint8_t b) {
  brightness = b;
#ifdef ESP32
  ledcWrite(1, brightness);
#else
  digitalWrite(TFT_LED, brightness);
#endif
}

void OswHal::increaseBrightness(uint8_t v) {
  if (brightness > 255 - v) {
    brightness = 255;
  } else {
    brightness += v;
  }
};
void OswHal::decreaseBrightness(uint8_t v) {
  if (brightness < v) {
    brightness = 0;
  } else {
    brightness -= v;
  }
};
