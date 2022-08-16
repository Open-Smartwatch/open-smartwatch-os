#include <stdint.h>

#include <iostream>

#include "../../anim_water_ripple.h"
#include "../../gfx_2d.h"
#include "../../gfx_util.h"
#include "../../FakeArduino.h"
#include "../../FakeArduinoWindowSDL.h"

using namespace std;

#define BUF_W 240
#define BUF_H 240
#define WATER_W 120
#define WATER_H 120

int8_t wbuf1[WATER_W * WATER_H];
int8_t wbuf2[WATER_W * WATER_H];

Graphics2D gfx2d(BUF_W, BUF_H, 16);
Graphics2D waterBackground(WATER_W, WATER_H, 8);
Graphics2D waterScreenBuffer(WATER_W, WATER_H, 8);

// mix colors
int16_t mix(uint8_t base, float n1, float n2) { return (n1 - abs(n2)) * base; }

class WaterWindow : public SDLWindowRGB565 {
 public:
  WaterWindow(Graphics2D* gfx2d, int w, int h) : SDLWindowRGB565(gfx2d, w, h) {}
  void setup() {
    for (uint16_t x = 0; x < WATER_W; x++) {
      for (uint16_t y = 0; y < WATER_H; y++) {
        waterBackground.drawPixel(x, y, rgb565(x % 255, y % 255, 0));
      }
    }
  }

  void loop() {
    static uint16_t counter = 0;
    uint16_t r1 = random(WATER_W - 4);
    uint16_t r2 = random(WATER_H - 4);

    for (uint16_t x = r1; x < r1 + 3; x++) {
      for (uint16_t y = r2; y < r2 + 3; y++) {
        wbuf1[x + WATER_W * y] = 127;
      }
    }

    counter++;
    calcWater(wbuf1, wbuf2, WATER_W, WATER_H, .9);
    mapWater(wbuf1, WATER_W, WATER_H, &waterBackground, &waterScreenBuffer, 0, 0);
    std::swap(wbuf1, wbuf2);

    gfx2d.drawGraphics2D_2x(0, 0, &waterScreenBuffer);

    delay(1000 / 30);
  }
};

int main(int argc, char* argv[]) {
  WaterWindow pw(&gfx2d, BUF_W, BUF_H);
  pw.run();
  return 0;
}
