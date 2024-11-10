#pragma once

#include <Arduino.h>
#include <gfx_2d.h>

#include <OswImage.h>

class OswIcon {
  public:
    static const unsigned int baseDimensions = 16; // every icon (regardless of source size) shall be treated as a 16x16 grid
    uint16_t color;

    OswIcon(uint16_t color): color(color) {};

    virtual void draw(Graphics2D* gfx, int x, int y, float scale = 1, OswImage::Alignment xAlign = OswImage::Alignment::START, OswImage::Alignment yAlign = OswImage::Alignment::START) const = 0;
};
