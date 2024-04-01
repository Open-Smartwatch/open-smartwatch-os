#include "Arduino_Canvas_Graphics2D.h"

#ifndef OSW_EMULATOR
#include <Arduino_DataBus.h>
#endif
#include <gfx_2d_print.h>

Arduino_Canvas_Graphics2D::Arduino_Canvas_Graphics2D(int16_t w, int16_t h, Arduino_G* output, int16_t output_x, int16_t output_y)
      : Graphics2DPrint(w, h, DISP_CHUNK_H_LD, true), _output(output), _output_x(output_x), _output_y(output_y) 
    {}

void Arduino_Canvas_Graphics2D::flush() {
    // only flush if there is a buffer
    if (this->hasBuffer()) {
        uint8_t chunkHeight = 1 << chunkHeightLd;
        for (uint8_t chunk = 0; chunk < this->numChunks(); chunk++) {
            _output->draw16bitRGBBitmap(this->getChunkOffset(chunk), chunk * chunkHeight, this->getChunk(chunk),
                                        this->getChunkWidth(chunk), chunkHeight);
        }
    }
}
