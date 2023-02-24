#include <OswIcon.h>

#include <OswLogger.h>

OswIcon::OswIcon(const unsigned char* data, const unsigned char dimension, uint16_t color): color(color), data(data), dimension(dimension) {

}

void OswIcon::draw(Graphics2D* gfx, int x, int y, float scale, Alignment xAlign, Alignment yAlign) {
    // modify x and y based on alignment
    switch(xAlign) {
        case Alignment::START:
            break;
        case Alignment::CENTER:
            x -= dimension * scale / 2;
            break;
        case Alignment::END:
            x -= dimension * scale;
            break;
    }
    switch(yAlign) {
        case Alignment::START:
            break;
        case Alignment::CENTER:
            y -= dimension * scale / 2;
            break;
        case Alignment::END:
            y -= dimension * scale;
            break;
    }

    // each icon consists of a 1D array of bits, where each bit represents a pixel
    for(float u = x; u < x + dimension * scale; u++) {
        for(float v = y; v < y + dimension * scale; v++) {
            int bitindex = (int)((u - x) / scale) + (int)((v - y) / scale) * dimension;
            if(this->debugBackground)
                gfx->drawPixel(u, v, rgb565(80, 80, 80));
            if(data[bitindex / 8] & (1 << (bitindex % 8)))
                gfx->drawPixel(u, v, color);
        }
    }
}