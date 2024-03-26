#include "Arduino_Canvas_Graphics2D.h"

#ifndef OSW_EMULATOR
#include <Arduino_DataBus.h>
#endif
#include <gfx_2d_print.h>

#include "config_defaults.h"

Arduino_Canvas_Graphics2D::Arduino_Canvas_Graphics2D(int16_t w, int16_t h, Arduino_G* output, int16_t output_x,
        int16_t output_y)
    : Graphics2DPrint(w, h, DISP_CHUNK_H, true), _output(output), _output_x(output_x), _output_y(output_y) {}

void Arduino_Canvas_Graphics2D::begin(int32_t speed) {
    _output->begin(speed);
    // _output->fillScreen(BLACK);
}
void Arduino_Canvas_Graphics2D::writePixelPreclipped(int16_t x, int16_t y, uint16_t color) {
    this->drawPixel(x, y, color);
}
void Arduino_Canvas_Graphics2D::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    this->drawVLine(x, y, h, color);
}
void Arduino_Canvas_Graphics2D::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    this->drawHLine(x, y, w, color);
}
void Arduino_Canvas_Graphics2D::flush() {
    // only flush if there is a buffer
    if (this->hasBuffer()) {
        uint8_t chunkHeight = this->getChunkHeight();
        for (uint8_t chunk = 0; chunk < this->numChunks(); chunk++) {
            _output->draw16bitRGBBitmap(this->getChunkOffset(chunk), chunk * chunkHeight, this->getChunk(chunk),
                                        this->getChunkWidth(chunk), chunkHeight);
        }
    }
}
