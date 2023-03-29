#pragma once

#include <Arduino.h>
#include <gfx_2d.h>

#include <OswImage.h>

class OswIcon {
  public:
    static const unsigned int baseDimensions = 16;
    uint16_t color;

    OswIcon(const unsigned char* data, const unsigned char dimension, uint16_t color);

    void draw(Graphics2D* gfx, int x, int y, float scale = 1, OswImage::Alignment xAlign = OswImage::Alignment::START, OswImage::Alignment yAlign = OswImage::Alignment::START);
  private:
    static const bool debugBackground = false;

    const unsigned char* data;
    const unsigned char dimension;
};