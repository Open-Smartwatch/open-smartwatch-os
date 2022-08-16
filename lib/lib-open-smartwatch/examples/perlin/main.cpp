
#include <stdint.h>

#include <iostream>

#include "../../FakeArduino.h"
#include "../../FakeArduinoWindowSDL.h"
#include "../../gfx_2d.h"
#include "../../gfx_util.h"
#include "../../math_angles.h"
#include "../../SimplexNoise.h"

SimplexNoise sn;

using namespace std;

#define BUF_W 240
#define BUF_H 240
uint16_t screenBuffer[BUF_W * BUF_H];

Graphics2D gfx2d(BUF_W, BUF_H, 16);

// mix colors
int16_t mix(uint8_t base, float n1, float n2) { return (n1 - abs(n2)) * base; }
void fillPerlin(uint16_t counter) {
  for (uint8_t x = 0; x < BUF_W; x++) {
    for (uint8_t y = 0; y < BUF_H; y++) {
      float n1 = sn.fractal(7, (x + counter) / 120.0, y / 120.0);  // sn.noise returns [-1,1]
      float n2 = sn.noise((x + counter) / 2, y / 2);               // sn.noise returns [-1,1]

      uint8_t r = n1 > 0 ? (n1 - n2) * 255 : 0;
      uint8_t g = r;
      uint8_t b = r;

      uint16_t color = rgb565(r, g, b);

      if (n1 < -.1) {  // deep water
        color = rgb565(72 * (1 + n1) + mix(5, n1, n2), 72 * (1 + n1) + mix(5, n1, n2), 190 * (1 + n1) + mix(5, n1, n2));
      } else if (n1 < 0) {  // shallow water plateaus
        color = rgb565(72 + mix(5, n1, n2), 72 + mix(5, n1, n2), 190 + mix(5, n1, n2));
      } else if (n1 < .0125) {  // beaches
        color = rgb565(200 + mix(5, n1, n2), 200 + mix(20, n1, n2), 47 + mix(5, n1, n2));
      } else if (n1 < .2) {  // meadows
        color = rgb565(85 + mix(40, n1, n2), 107 + mix(20, n1, n2), 47 + mix(20, n1, n2));
      } else if (n1 < .7) {  // forest
        color = rgb565((85 * (1 - (n1 - .2) * 2)) + (35 * ((n1 - .2) * 2)) - mix(10, n1, n2),
                       (107 * (1 - (n1 - .2) * 2)) + (57 * ((n1 - .2) * 2)) - mix(20, n1, n2),
                       (47 * (1 - (n1 - .2) * 2)) + (7 * ((n1 - .2) * 2)) - mix(10, n1, n2));
      } else {
        color = rgb565(35, 57, 7) - rgb565(85 + mix(40, n1, n2), 107 + mix(20, n1, n2), 47 + mix(20, n1, n2));
      }

      gfx2d.drawPixel(x, y, color);
    }
  }
}
class PerlinWindow : public SDLWindowRGB565 {
 public:
  PerlinWindow(Graphics2D* gfx2d, int w, int h) : SDLWindowRGB565(gfx2d, w, h) {}
  void setup() { printf("zero is %d", rgb565(0, 0, 0)); }

  void loop() {
    static uint16_t counter = 0;
    counter++;
    fillPerlin(counter);
  }
};

int main(int argc, char* argv[]) {
  PerlinWindow pw(&gfx2d, BUF_W, BUF_H);
  pw.run();
  return 0;
}
