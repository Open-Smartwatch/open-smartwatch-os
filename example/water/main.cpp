#include <FakeArduino.h>
#include <SDLWindow.h>
#include <stdint.h>

#include <iostream>

#include "../../src/anim/water_ripple.h"
#include "../../src/gfx/p3dt_gfx_2d.h"
#include "../../src/gfx/p3dt_gfx_util.h"
using namespace std;

#define BUF_W 240
#define BUF_H 240
uint16_t screenBuffer[BUF_W * BUF_H];
int8_t buffer1[BUF_W * BUF_H];
int8_t buffer2[BUF_W * BUF_H];

Graphics2D gfx2d(BUF_W, BUF_H, 16);
Graphics2D someImage(BUF_W, BUF_H, 16);

// mix colors
int16_t mix(uint8_t base, float n1, float n2) { return (n1 - abs(n2)) * base; }

class WaterWindow : public SDLWindowRGB565 {
 public:
  WaterWindow(Graphics2D* gfx2d, int w, int h) : SDLWindowRGB565(gfx2d, w, h) {}
  void setup() {
    for (uint16_t x = 0; x < BUF_W; x++) {
      for (uint16_t y = 0; y < BUF_H; y++) {
        someImage.drawPixel(x, y, rgb565(x % 255, y % 255, 0));
      }
    }
  }

  void loop() {
    static uint16_t counter = 0;
    uint16_t r1 = random(BUF_W - 4);
    uint16_t r2 = random(BUF_H - 4);

    for (uint16_t x = r1; x < r1 + 3; x++) {
      for (uint16_t y = r2; y < r2 + 3; y++) {
        buffer1[x + BUF_W * y] = 127;
      }
    }

    counter++;
    calcWater(buffer1, buffer2, BUF_W, BUF_H, counter % 4 ? 1 : .99);
    mapWater(buffer1, BUF_W, BUF_H, &someImage, &gfx2d, 0, 0);
    std::swap(buffer1, buffer2);

    // uint8_t* swap = buffer1;
    // buffer1 = buffer2;
    // buffer2 = swap;
    delay(1000 / 30);
  }
};

int main(int argc, char* argv[]) {
  WaterWindow pw(&gfx2d, BUF_W, BUF_H);
  pw.run();
  return 0;
}
