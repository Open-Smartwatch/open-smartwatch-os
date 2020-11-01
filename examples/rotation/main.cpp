#include <stdint.h>
#include <stdio.h>

#include <iostream>

#include "../../FakeArduino.h"
#include "../../FakeArduinoWindowSDL.h"
#include "../../gfx_2d.h"
#include "../../gfx_util.h"

using namespace std;

#define BUF_W 240
#define BUF_H 240
#define IMG_W 32
#define IMG_H 32

Graphics2D gfx2d(BUF_W, BUF_H, 16);
Graphics2D backgroundImg(BUF_W, BUF_H, 16);
Graphics2D img1(IMG_W, IMG_H, 8);

// mix colors
int16_t mix(uint8_t base, float n1, float n2) { return (n1 - abs(n2)) * base; }

class RotationExampleWindow : public SDLWindowRGB565 {
 public:
  RotationExampleWindow(Graphics2D* gfx2d, int w, int h) : SDLWindowRGB565(gfx2d, w, h) {}
  void setup() {
    for (uint16_t x = 0; x < BUF_W; x++) {
      for (uint16_t y = 0; y < BUF_H; y++) {
        backgroundImg.drawPixel(x, y, rgb565(0, x, y));
      }
    }
    for (uint16_t x = 0; x < IMG_W; x++) {
      for (uint16_t y = 0; y < IMG_H; y++) {
        img1.drawPixel(x, y, rgb565(x * 8, y * 8, 0));
      }
    }
  }

  void loop() {
    static uint16_t counter = 1;
    counter++;

    gfx2d.drawGraphics2D(&backgroundImg, 0, 0);
    gfx2d.drawGraphics2D_rotated(&img1, 64, 64, 16, 16, counter / 15.0);

    rotateY(img1.getWidth() - 1, 0, 16, 16, counter / 15.0);
    rotateY(img1.getWidth() - 1, 0, 16, 16, counter / 15.0);

    gfx2d.drawGraphics2D_rotatedAdvanced(&img1, 64, 128, 16, 16, (counter / 15.0));

    delay(1000 / 30);
  }
};

int main(int argc, char* argv[]) {
  RotationExampleWindow exampleWindow(&gfx2d, BUF_W, BUF_H);
  exampleWindow.run();
  return 0;
}
