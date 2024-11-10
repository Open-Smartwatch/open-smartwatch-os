#pragma once
#include <OswIcon.h>

class OswIconProgmem: public OswIcon {
  public:
    OswIconProgmem(const unsigned char* data, const unsigned char dimension, uint16_t color);

    void draw(Graphics2D* gfx, int x, int y, float scale = 1, OswImage::Alignment xAlign = OswImage::Alignment::START, OswImage::Alignment yAlign = OswImage::Alignment::START) const override;
    void draw(Graphics2D* gfx, int x, int y, uint16_t color, float scale = 1, OswImage::Alignment xAlign = OswImage::Alignment::START, OswImage::Alignment yAlign = OswImage::Alignment::START) const;
  private:
    static const bool debugBackground = false;

    const unsigned char* data;
    const unsigned char dimension;
};
