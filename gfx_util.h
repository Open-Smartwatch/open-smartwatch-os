#ifndef P3DT_GFX_UTIL_H
#define P3DT_GFX_UTIL_H

#ifdef FAKE_ARDUINO
#include "FakeArduino.h"
#else
#include <Arduino.h>
#endif


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

uint16_t blend(uint16_t target, uint16_t source, float alpha);
uint16_t dimColor(uint16_t oc, uint8_t amount);
uint16_t changeColor(uint16_t oc, float amount);

void hsvToRgb(const unsigned char& h, const unsigned char& s, const unsigned char& v, unsigned char& r,
              unsigned char& g, unsigned char& b);

void rgbToHsv(const unsigned char& r, const unsigned char& g, const unsigned char& b, unsigned char& h,
              unsigned char& s, unsigned char& v);
#endif
