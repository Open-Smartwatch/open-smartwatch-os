#include "gfx_util.h"

#include <Arduino.h>

uint16_t blend(uint16_t target, uint16_t source, float alpha) {
    uint8_t r = rgb565_red(source) * alpha + rgb565_red(target) * (1.0 - alpha);
    uint8_t g = rgb565_green(source) * alpha + rgb565_green(target) * (1.0 - alpha);
    uint8_t b = rgb565_blue(source) * alpha + rgb565_blue(target) * (1.0 - alpha);

    return rgb565(r, g, b);
}

// optimized integer version
uint16_t blend(uint16_t target, uint16_t source, uint8_t alpha) {
    const uint8_t a_inv = 255 - alpha;

    uint8_t r = (rgb565_red(source) * alpha + rgb565_red(target) * a_inv) / 255;
    uint8_t g = (rgb565_green(source) * alpha + rgb565_green(target) * a_inv) / 255;
    uint8_t b = (rgb565_blue(source) * alpha + rgb565_blue(target) * a_inv) / 255;
    
    return rgb565(r, g, b);
}

/**
 * @brief Calculated the color code of a dimmed color
 *
 * @param oc Color code
 * @param amount Amount to dimmed.
 * @return uint16_t Color code of the dimmed color.
 */
uint16_t dimColor(uint16_t oc, uint8_t amount) {
    uint16_t r = rgb565_red(oc);
    uint16_t g = rgb565_green(oc);
    uint16_t b = rgb565_blue(oc);
    r = r >= amount ? r - amount : 0;
    g = g >= amount * 2 ? g - amount * 2 : 0;
    b = b >= amount ? b - amount : 0;

    uint16_t nc = rgb565(r, g, b);
    return nc;
}

uint16_t changeColor(uint16_t oc, float amount) {
    uint16_t r = rgb565_red(oc);
    uint16_t g = rgb565_green(oc);
    uint16_t b = rgb565_blue(oc);
    r = r * amount;
    g = g * amount;
    b = b * amount;

    uint16_t nc = rgb565(r, g, b);
    return nc;
}

// Shamelessly copied from
// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
void hsvToRgb(const unsigned char& h, const unsigned char& s, const unsigned char& v, unsigned char& r,
              unsigned char& g, unsigned char& b) {
    unsigned char region, remainder, p, q, t;

    if (s == 0) {
        r = v;
        g = v;
        b = v;
        return;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
    case 0:
        r = v;
        g = t;
        b = p;
        break;
    case 1:
        r = q;
        g = v;
        b = p;
        break;
    case 2:
        r = p;
        g = v;
        b = t;
        break;
    case 3:
        r = p;
        g = q;
        b = v;
        break;
    case 4:
        r = t;
        g = p;
        b = v;
        break;
    default:
        r = v;
        g = p;
        b = q;
        break;
    }
}

// Also shamelessly copied from
// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
void rgbToHsv(const unsigned char& r, const unsigned char& g, const unsigned char& b, unsigned char& h,
              unsigned char& s, unsigned char& v) {
    unsigned char rgbMin, rgbMax;

    rgbMin = r < g ? (r < b ? r : b) : (g < b ? g : b);
    rgbMax = r > g ? (r > b ? r : b) : (g > b ? g : b);

    v = rgbMax;
    if (v == 0) {
        h = 0;
        s = 0;
        return;
    }

    s = 255 * long(rgbMax - rgbMin) / v;
    if (s == 0) {
        h = 0;
        return;
    }

    if (rgbMax == r)
        h = 0 + 43 * (g - b) / (rgbMax - rgbMin);
    else if (rgbMax == g)
        h = 85 + 43 * (b - r) / (rgbMax - rgbMin);
    else
        h = 171 + 43 * (r - g) / (rgbMax - rgbMin);
};
