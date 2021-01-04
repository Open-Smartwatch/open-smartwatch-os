#include "gfx_util.h"

#ifdef FAKE_ARDUINO
#include "FakeArduino.h"
#else
#include <Arduino.h>
#endif

uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue) {
  return ((red & 0b00011111000) << 8) | ((green & 0b00011111100) << 3) | (blue >> 3);
}

uint16_t blend(uint16_t target, uint16_t source, float alpha) {
  uint8_t r = rgb565_red(source) * alpha + rgb565_red(target) * (1.0 - alpha);
  uint8_t g = rgb565_green(source) * alpha + rgb565_green(target) * (1.0 - alpha);
  uint8_t b = rgb565_blue(source) * alpha + rgb565_blue(target) * (1.0 - alpha);

  return rgb565(r, g, b);
}

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

uint8_t rgb565_red(uint16_t rgb565) {
  // |rrrrrggg|gggbbbbb|
  return (rgb565 >> 8) & 0b11111000;
}

uint8_t rgb565_green(uint16_t rgb565) {
  // |rrrrr666|666bbbbb|
  return (rgb565 >> 3) & 0b11111100;
}

uint8_t rgb565_blue(uint16_t rgb565) {
  // |rrrrr666|666bbbbb|
  return (rgb565 << 3);
}
