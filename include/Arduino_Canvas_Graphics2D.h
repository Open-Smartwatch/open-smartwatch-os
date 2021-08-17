#ifndef ArduinoGraphics2DCanvas_H
#define ArduinoGraphics2DCanvas_H

#include <Arduino_GFX.h>
#include <gfx_2d_print.h>

class Arduino_Canvas_Graphics2D : public Arduino_GFX {
 public:
  Arduino_Canvas_Graphics2D(int16_t w, int16_t h, Arduino_G *output, int16_t output_x = 0, int16_t output_y = 0);

  void begin(int32_t speed = 0);
  void writePixelPreclipped(int16_t x, int16_t y, uint16_t color);

  /**
   * @brief Draw a vertical line from the point (x,y) with h height
   *
   * @param x x-coordinates in pixel of the beginning point
   * @param y y-coordinates in pixel of the beginning point
   * @param h height of the vertical line in pixel.
   * @param color color code of the line
   */
  void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);

  /**
   * @brief Draw an horizontal line from the point (x,y) with w width
   *
   * @param x x-coordinates in pixel of the beginning point
   * @param y y-coordinates in pixel of the beginning point
   * @param w width of the vertical line in pixel.
   * @param color color code of the line
   */
  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

  /**
   * @brief Flush the buffer to update the display
   *
   */
  void flush(void);

  Graphics2DPrint *getGraphics2D(void);

 protected:
  Graphics2DPrint *_gfx2d;
  Arduino_G *_output;
  int16_t _output_x, _output_y;

 private:
};

#endif
