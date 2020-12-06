#ifndef P3DT_GFX_UTIL_H
#define P3DT_GFX_UTIL_H

#ifdef FAKE_ARDUINO
#include "FakeArduino.h"
#else
#include <Arduino.h>
#endif

uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue);
uint16_t blend(uint16_t target, uint16_t source, float alpha);
uint8_t rgb565_red(uint16_t rgb565);
uint8_t rgb565_green(uint16_t rgb565);
uint8_t rgb565_blue(uint16_t rgb565);

#endif