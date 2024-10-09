#pragma once

#include <Arduino.h>
#include <gfx_2d.h>
#include <pngle.h>

class OswImage {
  public:
    enum class Alignment {
        START,
        CENTER,
        END
    };

    OswImage(const unsigned char* data, unsigned int length, unsigned short width, unsigned short height);

    void draw(Graphics2D* gfx, int x, int y, float scale = 1, Alignment xAlign = Alignment::START, Alignment yAlign = Alignment::START);
  private:
    static Graphics2D* cbGfx;
    static unsigned int cbOffX;
    static unsigned int cbOffY;
    static Alignment cbAlignX;
    static Alignment cbAlignY;
    static float cbScale;
    const unsigned char* data;
    const unsigned int length;
    const unsigned short width;
    const unsigned short height;

    static void drawCallback(pngle_t* pngle, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char rgba[4]);
};
