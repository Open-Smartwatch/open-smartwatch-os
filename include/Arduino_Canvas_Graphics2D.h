#ifndef ArduinoGraphics2DCanvas_H
#define ArduinoGraphics2DCanvas_H

#include <Arduino_GFX.h>
#include <gfx_2d_print.h>
#include "config_defaults.h"

class Arduino_Canvas_Graphics2D : public Graphics2DPrint {
  public:
    Arduino_Canvas_Graphics2D(int16_t w, int16_t h, Arduino_G* output, int16_t output_x = 0, int16_t output_y = 0);

    /**
     * DIFFERENCES TO THE ORIGINAL Arduino_GFX library:
     * * setCursor -> setTextCursor
     * * fillScreen -> fill
     * * drawArc has an entirely different function signature
     * * fillRoundRect -> fillRFrame
     * * fillRect -> fillFrame
     * * drawRect -> drawFrame
     * * drawFastHLine, drawFastVLine are used internally, please use their implicit "non-fast" variants: drawHLine, drawVLine
     *
     * Why? Because our emulator needs to also use this library and as Arduino_GFX does NOT support building on non-embedded devices,
     * we have copy-pasted this utility together...
     */

    void flush();

    inline void begin(int32_t speed = GFX_NOT_DEFINED) {
        _output->begin(speed);
        // _output->fillScreen(BLACK);
    }

    inline void writePixelPreclipped(int16_t x, int16_t y, uint16_t color) {
        this->drawPixel(x, y, color);
    }
    inline void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
        this->drawVLine(x, y, h, color);
    }
    inline void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
        this->drawHLine(x, y, w, color);
    }

  protected:
    Arduino_G* _output;
    int16_t _output_x, _output_y;

  private:
};

#endif
