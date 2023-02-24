#pragma once

#include <Arduino.h>
#include <gfx_2d.h>

class OswIcon {
  public:
    enum class Alignment {
        START,
        CENTER,
        END
    };
    uint16_t color;

    OswIcon(const unsigned char* data, const unsigned char dimension, uint16_t color);

    void draw(Graphics2D* gfx, int x, int y, float scale = 1, Alignment xAlign = Alignment::START, Alignment yAlign = Alignment::START);
  private:
    static const bool debugBackground = false;

    const unsigned char* data;
    const unsigned char dimension;
};