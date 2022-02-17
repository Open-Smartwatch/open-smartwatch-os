#pragma once

#include <Arduino_GFX.h>
#include <gfx_2d_print.h>
#include <gfx_2d.h>

#include "Defines.h"

class Arduino_Canvas_Graphics2D : public Arduino_GFX {
 public:
  Arduino_Canvas_Graphics2D(int16_t w, int16_t h, Arduino_G *output, int16_t output_x = 0, int16_t output_y = 0) :
  Arduino_GFX(w, h, 16, false, false) {};


  void setTextColor(uint16_t c) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
    uint16_t fg = c;
    uint16_t bg = 0;
    Arduino_GFX::setTextColor(fg, bg);
  }

  void setTextColor(int fg, int bg) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
    Arduino_GFX::setTextColor(fg, bg);
  }

  void setCursor(int16_t x, int16_t y) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
    Arduino_GFX::setTextCursor(x, y);
  }

  void fillScreen(uint16_t c) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
  }

  void drawArc(int, int, int, const int&, int, int, uint16_t) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
  }

  void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
  }

  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
  }

  void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
  }

  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
  }

  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
  }

  void begin(int32_t speed = 0) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
  };
  void writePixelPreclipped(int16_t x, int16_t y, uint16_t color) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
  };
  void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
  };
  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
  };
  void flush(void) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
  };

  Graphics2DPrint *getGraphics2D() {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
    return nullptr;
  };

 protected:
  Graphics2DPrint *_gfx2d;
  Arduino_G *_output;
  int16_t _output_x, _output_y;

 private:
};