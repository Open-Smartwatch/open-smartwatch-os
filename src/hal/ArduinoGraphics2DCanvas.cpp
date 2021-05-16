#include "ArduinoGraphics2DCanvas.h"

#include <Arduino_Canvas.h>
#include <Arduino_DataBus.h>
#include <Arduino_GFX.h>
#include <gfx_2d_print.h>

#include "config.h"

ArduinoGraphics2DCanvas::ArduinoGraphics2DCanvas(int16_t w, int16_t h, Arduino_G* output, int16_t output_x,
                                                 int16_t output_y)
    : Arduino_GFX(w, h), _output(output), _output_x(output_x), _output_y(output_y) {
      wrap = false; //Disable text wrapping
    }

void ArduinoGraphics2DCanvas::begin(uint32_t speed) {
  _gfx2d = new Graphics2DPrint(_width, _height, DISP_CHUNK_H, true);
  _output->begin(speed);
  //   _output->fillScreen(BLACK);
}
void ArduinoGraphics2DCanvas::writePixelPreclipped(int16_t x, int16_t y, uint16_t color) {
  _gfx2d->drawPixel(x, y, color);
}
void ArduinoGraphics2DCanvas::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  _gfx2d->drawVLine(x, y, h, color);
}
void ArduinoGraphics2DCanvas::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  _gfx2d->drawHLine(x, y, w, color);
}
void ArduinoGraphics2DCanvas::flush(void) {
  // only flush if there is a buffer
  if (_gfx2d->hasBuffer()) {
    uint8_t chunkHeight = _gfx2d->getChunkHeight();
    for (uint8_t chunk = 0; chunk < _gfx2d->numChunks(); chunk++) {
      _output->draw16bitRGBBitmap(_gfx2d->getChunkOffset(chunk), chunk * chunkHeight, _gfx2d->getChunk(chunk),
                                  _gfx2d->getChunkWidth(chunk), chunkHeight);
    }
  }
  
}

Graphics2DPrint* ArduinoGraphics2DCanvas::getGraphics2D(void) { return _gfx2d; }
