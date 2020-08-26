#ifndef P3DT_GFX_UTIL_H
#define P3DT_GFX_UTIL_H

uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue) {
  return ((red & 0b11111000) << 8) | ((green & 0b11111100) << 3) | (blue >> 3);
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

#endif