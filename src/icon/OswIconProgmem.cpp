#include <icon/OswIconProgmem.h>

#include <OswLogger.h>

OswIconProgmem::OswIconProgmem(const unsigned char* data, const unsigned char dimension, uint16_t color): OswIcon(color), data(data), dimension(dimension) {

}

void OswIconProgmem::draw(Graphics2D* gfx, int x, int y, uint16_t color, float scale, OswImage::Alignment xAlign, OswImage::Alignment yAlign) const {
    // modify x and y based on alignment
    scale *= (float) this->baseDimensions / this->dimension; // treat all icons as a 16x16 grid
    switch(xAlign) {
    case OswImage::Alignment::START:
        break;
    case OswImage::Alignment::CENTER:
        x -= dimension * scale / 2;
        break;
    case OswImage::Alignment::END:
        x -= dimension * scale;
        break;
    }
    switch(yAlign) {
    case OswImage::Alignment::START:
        break;
    case OswImage::Alignment::CENTER:
        y -= dimension * scale / 2;
        break;
    case OswImage::Alignment::END:
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

void OswIconProgmem::draw(Graphics2D* gfx, int x, int y, float scale, OswImage::Alignment xAlign, OswImage::Alignment yAlign) const {
    this->draw(gfx, x, y, this->color, scale, xAlign, yAlign);
}
