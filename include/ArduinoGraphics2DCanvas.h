#ifndef ArduinoGraphics2DCanvas_H
#define ArduinoGraphics2DCanvas_H

#include <Arduino_DataBus.h>
#include <Arduino_GFX.h>
#include <gfx_2d_print.h>

class ArduinoGraphics2DCanvas : public Arduino_GFX {
 public:
  ArduinoGraphics2DCanvas(int16_t w, int16_t h, Arduino_G *output, int16_t output_x = 0, int16_t output_y = 0);

  virtual void begin(uint32_t speed = 0);
  virtual void writePixelPreclipped(int16_t x, int16_t y, uint16_t color);
  virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  virtual void flush(void);

  Graphics2DPrint *getGraphics2D(void);

 protected:
  Graphics2DPrint *_gfx2d;
  Arduino_G *_output;
  int16_t _output_x, _output_y;

 private:
};

#endif
