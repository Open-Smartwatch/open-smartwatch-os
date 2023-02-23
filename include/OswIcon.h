#pragma once

#include <Arduino.h>
#include <gfx_2d.h>

class OswIcon {
  public:
    OswIcon(unsigned char* data, uint16_t color);

    void draw(Graphics2D* gfx, int x, int y);
  private:
    unsigned char* data;
    uint16_t color;
};