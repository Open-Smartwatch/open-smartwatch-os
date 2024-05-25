#include <OswImage.h>

#include <OswLogger.h>

Graphics2D* OswImage::cbGfx = nullptr;
unsigned int OswImage::cbOffX = 0;
unsigned int OswImage::cbOffY = 0;
float OswImage::cbScale = 0;
OswImage::Alignment OswImage::cbAlignX = OswImage::Alignment::START;
OswImage::Alignment OswImage::cbAlignY = OswImage::Alignment::START;

OswImage::OswImage(const unsigned char* data, unsigned int length, unsigned short width, unsigned short height): data(data), length(length), width(width), height(height) {

}

/**
 * @brief Draws the image to the given Graphics2D instance - somewhat slow (because the PNG is decoded on the fly)
 *
 * @param gfx
 * @param x
 * @param y
 * @param scale Dirty hack to scale the image - this is not a proper implementation of scaling and will not work for > 1.0!
 * @param xAlign
 * @param yAlign
 */
void OswImage::draw(Graphics2D* gfx, int x, int y, float scale, Alignment xAlign, Alignment yAlign) {
    pngle_t* pngle = pngle_new();
    OswImage::cbGfx = gfx;
    OswImage::cbOffX = x;
    OswImage::cbOffY = y;
    OswImage::cbAlignX = xAlign;
    OswImage::cbAlignY = yAlign;
    OswImage::cbScale = scale;
    switch(OswImage::cbAlignX) {
    case OswImage::Alignment::START:
        break;
    case OswImage::Alignment::CENTER:
        OswImage::cbOffX -= this->width * scale / 2;
        break;
    case OswImage::Alignment::END:
        OswImage::cbOffX -= this->width * scale;
        break;
    }
    switch(OswImage::cbAlignY) {
    case OswImage::Alignment::START:
        break;
    case OswImage::Alignment::CENTER:
        OswImage::cbOffY -= this->height * scale / 2;
        break;
    case OswImage::Alignment::END:
        OswImage::cbOffY -= this->height * scale;
        break;
    }
    pngle_set_draw_callback(pngle, OswImage::drawCallback);
    if (pngle_feed(pngle, this->data, this->length) < 0)
        OSW_LOG_E(pngle_error(pngle)); // Whoopsie!
    pngle_destroy(pngle);
}

void OswImage::drawCallback(pngle_t* pngle, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char rgba[4]) {
    const unsigned char r = rgba[0];  // 0 - 255
    const unsigned char g = rgba[1];  // 0 - 255
    const unsigned char b = rgba[2];  // 0 - 255
    const unsigned char a = rgba[3];  // 0: fully transparent, 255: fully opaque
    const float scale = OswImage::cbScale;

    // We pretty much ignore alpha - and just draw the pixel if it's not transparent
    if (a > 0)
        OswImage::cbGfx->drawPixel(OswImage::cbOffX + x * scale, OswImage::cbOffY + y * scale, rgb565(r, g, b));
}
