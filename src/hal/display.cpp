#include <Arduino_GFX.h>
#include <databus/Arduino_ESP32SPI.h>
#include <display/Arduino_GC9A01.h>
#include <gfx_2d_print.h>
#include <gfx_util.h>
#include <math_osm.h>
#include <pngle.h>

#include "Arduino_Canvas_Graphics2D.h"
#include "config_defaults.h"
#include "osw_hal.h"
#include "osw_pins.h"

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO, VSPI /* spi_num */);
#if defined(GPS_EDITION_ROTATED)
Arduino_GC9A01 *tft = new Arduino_GC9A01(bus, TFT_RST, 1 /* rotation */, true /* IPS */);
#else
Arduino_GC9A01 *tft = new Arduino_GC9A01(bus, TFT_RST, 0 /* rotation */, true /* IPS */);
#endif
Arduino_Canvas_Graphics2D *canvas = new Arduino_Canvas_Graphics2D(DISP_W, DISP_H, tft);

class PixelPainter : public DrawPixel {
 public:
  PixelPainter(){};
  void drawPixel(int32_t x, int32_t y, uint16_t color) {
    // draw pixel
    tft->drawPixel(x, y, color);
  };
};
PixelPainter *pixelPainter = new PixelPainter();

void OswHal::requestDisableDisplayBuffer() { _requestDisableBuffer = true; }
void OswHal::requestEnableDisplayBuffer() { _requestEnableBuffer = true; }
void OswHal::disableDisplayBuffer() {  //
  canvas->getGraphics2D()->disableBuffer(pixelPainter);
}
void OswHal::enableDisplayBuffer() {  //
  canvas->getGraphics2D()->enableBuffer();
}

void OswHal::setupDisplay() {
#ifdef ESP32
  // nasty hack to avoid display flicker
  ledcAttachPin(TFT_LED, 1);
  ledcSetup(1, 12000, 8);  // 12 kHz PWM, 8-bit resolution
  ledcWrite(1, 0);
#endif

  canvas->begin(0);
  tft->displayOn();
  _screenOnSince = millis();
}

Arduino_TFT *OswHal::getArduino_TFT(void) { return tft; }
Arduino_Canvas_Graphics2D *OswHal::getCanvas(void) { return canvas; }
Graphics2DPrint *OswHal::gfx(void) { return canvas->getGraphics2D(); }

void OswHal::requestFlush(void) { _requestFlush = true; }
bool OswHal::isRequestFlush(void) { return _requestFlush; }

void OswHal::flushCanvas(void) {
  _requestFlush = false;
  canvas->flush();
}

void OswHal::displayOff(void) {
#ifdef ESP32
  ledcDetachPin(TFT_LED);
#endif
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, LOW);
  tft->displayOff();
  _screenOffSince = millis();
}

unsigned long OswHal::screenOnTime() { return millis() - _screenOnSince; }
unsigned long OswHal::screenOffTime() { return millis() - _screenOffSince; }

void OswHal::displayOn() {
  _screenOnSince = millis();
#ifdef ESP32
  ledcAttachPin(TFT_LED, 1);
  ledcSetup(1, 12000, 8);  // 12 kHz PWM, 8-bit resolution
#endif
  setBrightness(OswConfigAllKeys::settingDisplayBrightness.get());
  tft->displayOn();
}

void OswHal::setBrightness(uint8_t b) {
  _brightness = b;
#ifdef ESP32
  ledcWrite(1, _brightness);
  OswConfig::getInstance()->enableWrite();
  OswConfigAllKeys::settingDisplayBrightness.set(_brightness);
  OswConfig::getInstance()->disableWrite();
#else
  digitalWrite(TFT_LED, brightness);
#endif
#ifndef NDEBUG
  Serial.println("Setting brightness to " + String(b));
#endif
}

void OswHal::increaseBrightness(uint8_t v) {
  uint8_t new_brightness = 0;

  if(_brightness == 0){
    _brightness = OswHal::screenBrightness(true);
  }

  if (_brightness > 255 - v) {
    new_brightness = 255;
  } else {
    new_brightness = _brightness + v;
  }
  setBrightness(new_brightness);
};

void OswHal::decreaseBrightness(uint8_t v) {
  uint8_t new_brightness = 0;

  if(_brightness == 0){
    _brightness = OswHal::screenBrightness(true);
  }

  short minBrightness = 0;
#ifdef DISPLAY_MIN_BRIGHTNESS
  minBrightness = DISPLAY_MIN_BRIGHTNESS;
#endif

  if (minBrightness < 0) {
    minBrightness = 0;
  }
  if ((_brightness - v) < minBrightness) {
    new_brightness = minBrightness;
  } else {
    new_brightness = _brightness - v;
  }
  setBrightness(new_brightness);
};

uint8_t OswHal::screenBrightness(bool checkHardware) { 
  uint8_t screen_brightness = 0;

  if(checkHardware) {
    #ifdef ESP32
      screen_brightness = ledcRead(1);
    #else
      screen_brightness = digitalRead(TFT_LED);
    #endif
    _brightness = screen_brightness;
  }
  return _brightness; 
}
