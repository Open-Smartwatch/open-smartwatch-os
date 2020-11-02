#include <stdint.h>
#include <stdio.h>

#include "../../FakeArduino.h"
#include "../../FakeArduinoWindowSDL.h"
#include "../../gfx_2d.h"
#include "../../gfx_util.h"

using namespace std;

#define BUF_W 240
#define BUF_H 240
#define IMG_W 24
#define IMG_H 240

Graphics2D gfx2d(BUF_W, BUF_H, 16);
Graphics2D backgroundImg(BUF_W, BUF_H, 16);
Graphics2D handleMinute(IMG_W, IMG_H, 8);
Graphics2D handleHour(IMG_W, IMG_H, 8);

// mix colors
int16_t mix(uint8_t base, float n1, float n2) { return (n1 - abs(n2)) * base; }

class RotationExampleWindow : public SDLWindowRGB565 {
 public:
  RotationExampleWindow(Graphics2D* gfx2d, int w, int h) : SDLWindowRGB565(gfx2d, w, h) {}
  void setup() {
    loadPNG(&backgroundImg, "../watchface01.png");
    loadPNG(&handleMinute, "../watchface01m.png");
    loadPNG(&handleHour, "../watchface01h.png");

    gfx2d.setMaskEnabled(true);  // default (0,0,0)
    // gfx2d.setMaskColor(rgb565(255, 255, 255));
  }

  void loop() {
    static uint16_t counter = 1;
    counter++;

    gfx2d.drawGraphics2D(&backgroundImg, 0, 0);

    rotateY(handleMinute.getWidth() - 1, 0, 16, 16, counter / 15.0, 1);
    rotateY(handleMinute.getWidth() - 1, 0, 16, 16, counter / 15.0, 1);

    gfx2d.drawGraphics2D_rotated(&handleHour, 120, 120, 12, 120, -(counter / 5.0) / 60);
    gfx2d.drawGraphics2D_rotated(&handleMinute, 120, 120, 12, 120, -(counter / 5.0));

    delay(1000 / 30);
  }
};

int main(int argc, char* argv[]) {
  RotationExampleWindow exampleWindow(&gfx2d, BUF_W, BUF_H);
  exampleWindow.run();
  return 0;
}
