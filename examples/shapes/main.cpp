#include <stdint.h>

#include <iostream>

#include "../../FakeArduino.h"
#include "../../FakeArduinoWindowSDL.h"
#include "../../anim_doom_fire.h"
#include "../../gfx_2d.h"
#include "../../gfx_util.h"
#include "../../math_angles.h"
using namespace std;

#define BUF_W 240
#define BUF_H 240
uint16_t screenBuffer[BUF_W * BUF_H];

Graphics2D gfx2d(BUF_W, BUF_H, 16);

class WatchSimpleWindow : public SDLWindowRGB565 {
 public:
  WatchSimpleWindow(Graphics2D* gfx2d, int w, int h) : SDLWindowRGB565(gfx2d, w, h) {}
  void setup() {}

  void loop() {
    uint8_t cx = 119;
    uint8_t cy = 119;

    gfx2d.fill(rgb565(255, 0, 0));

    // gfx2d.drawLine(0, 0, x, x, rgb565(x, x, 255));
    // gfx2d.fillFrame(10, 10, 10, 10, rgb565(x, x, 100));
    // gfx2d.drawCircle(120, 120, 32, rgb565(x, 255, 255));
    // gfx2d.fillFrame(10, 10, 10, 10, rgb565(255, x, 100));
    // gfx2d.drawEllipse(20, 20, 5, 20, rgb565(255, 0, x));
    // gfx2d.fillEllipse(120, 20, 5, 20, rgb565(255, 0, x));
    // gfx2d.fillFrame(10, 10, 10, 10, rgb565(255, 255, 100));
    // gfx2d.drawEllipse(20, 20, 5, 20, rgb565(255, x, 0));
    // gfx2d.fillEllipse(120, 20, 5, 20, rgb565(255, 0, x));
    // gfx2d.fillRFrame(40, 40, 20, 30, 4, rgb565(255, x, 100));
    // gfx2d.drawRFrame(60, 60, 30, 20, 4, rgb565(255, x, x));

    gfx2d.drawCircle(119, 119, 119, rgb565(255, 255, 255));
    gfx2d.drawCircle(119, 120, 119, rgb565(255, 255, 255));
    gfx2d.drawCircle(120, 119, 119, rgb565(255, 255, 255));
    gfx2d.drawCircle(120, 120, 119, rgb565(255, 255, 255));

    uint8_t chunkHeight = 4;
    uint8_t width = gfx2d.getWidth();
    uint8_t height = gfx2d.getHeight();

    for (uint16_t y = 0; y < height; y += chunkHeight) {
      float y1 = (y + (y < height / 2 ? chunkHeight : 0)) - height / 2.0;
      float d = sqrt(120 * 120 - y1 * y1);
      uint16_t xOffset = 120 - d;
      uint16_t chunkWidth = ceil(d * 2);
      gfx2d.fillFrame(xOffset, y, chunkWidth, chunkHeight, rgb565(0, 0, 0));
    }
  }
};

int main(int argc, char* argv[]) {
  WatchSimpleWindow wsw(&gfx2d, BUF_W, BUF_H);
  wsw.run();
  return 0;
}
