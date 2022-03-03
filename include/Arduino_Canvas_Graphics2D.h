#ifndef ArduinoGraphics2DCanvas_H
#define ArduinoGraphics2DCanvas_H

#include <Arduino_GFX.h>
#include <gfx_2d_print.h>

class Arduino_Canvas_Graphics2D : public Graphics2DPrint {
 public:
  Arduino_Canvas_Graphics2D(int16_t w, int16_t h, Arduino_G *output, int16_t output_x = 0, int16_t output_y = 0);

  /**
   * DIFFERENCES TO THE ORIGINAL Arduino_GFX library:
   * * setCursor -> setTextCursor
   * * fillScreen -> fill
   * * drawArc has an entirely differnt function signature
   * * fillRoundRect -> fillRFrame
   * * fillRect -> fillFrame
   * * drawRect -> drawFrame
   * * drawFastHLine, drawFastVLine are used internally, please use their implicit "non-fast" variants: drawHLine, drawVLine
   * 
   * Why? Because our emulator needs to also use this library and as Arduino_GFX does NOT support building on non-embedded devices,
   * we have copy-pasted this utility together...
   */

  void begin(int32_t speed = 0);
  void writePixelPreclipped(int16_t x, int16_t y, uint16_t color);
  void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void flush(void);

 protected:
  Arduino_G *_output;
  int16_t _output_x, _output_y;

 private:
};

#endif
