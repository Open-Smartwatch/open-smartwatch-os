#ifndef P3DT_GFX_UTIL_H
#define P3DT_GFX_UTIL_H

#include <Arduino.h>

uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue);
uint32_t rgb888(uint8_t red, uint8_t green, uint8_t blue);

uint32_t rgb565to888(uint16_t rgb565);
uint16_t rgb888to565(uint32_t rgb888);

uint8_t rgb565_red(uint16_t rgb565);
uint8_t rgb565_green(uint16_t rgb565);
uint8_t rgb565_blue(uint16_t rgb565);

uint8_t rgb888_red(uint32_t rgb888);
uint8_t rgb888_green(uint32_t rgb888);
uint8_t rgb888_blue(uint32_t rgb888);

inline uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue) {
    return ((red & 0b00011111000) << 8) | ((green & 0b00011111100) << 3) | (blue >> 3);
}

inline uint32_t rgb888(uint8_t red, uint8_t green, uint8_t blue) {
    return ((uint32_t)red << 16) | ((uint32_t)green << 8) | (uint32_t)blue;
}

inline uint16_t rgb888to565(uint32_t rgb888) {
    return rgb565(rgb888_red(rgb888), rgb888_green(rgb888), rgb888_blue(rgb888));
}

inline uint32_t rgb565to888(uint16_t rgb565) {
    return rgb888(rgb565_red(rgb565), rgb565_green(rgb565), rgb565_blue(rgb565));
}

inline uint8_t rgb565_red(uint16_t rgb565) {
    // |rrrrrggg|gggbbbbb|
    return (rgb565 >> 8) & 0b11111000;
}

inline uint8_t rgb565_green(uint16_t rgb565) {
    // |rrrrrggg|gggbbbbb|
    return (rgb565 >> 3) & 0b11111100;
}

inline uint8_t rgb565_blue(uint16_t rgb565) {
    // |rrrrrggg|gggbbbbb|
    return (rgb565 << 3);
}

inline uint8_t rgb888_red(uint32_t rgb888) {
    // |rrrrrrrr|gggggggg|bbbbbbbb|
    return rgb888 >> 16;
}

inline uint8_t rgb888_green(uint32_t rgb888) {
    // |rrrrrrrr|gggggggg|bbbbbbbb|
    return rgb888 >> 8;
}

inline uint8_t rgb888_blue(uint32_t rgb888) {
    // |rrrrrrrr|gggggggg|bbbbbbbb|
    return rgb888;
}

uint16_t blend(uint16_t target, uint16_t source, uint8_t alpha);
uint16_t blend(uint16_t target, uint16_t source, float alpha);
uint16_t dimColor(uint16_t oc, uint8_t amount);
uint16_t changeColor(uint16_t oc, float amount);

void hsvToRgb(const unsigned char& h, const unsigned char& s, const unsigned char& v, unsigned char& r,
              unsigned char& g, unsigned char& b);

void rgbToHsv(const unsigned char& r, const unsigned char& g, const unsigned char& b, unsigned char& h,
              unsigned char& s, unsigned char& v);
#endif
