#include <stdint.h>
#include <stdio.h>

#include "../../FakeArduino.h"
#include "../../FakeArduinoWindowSDL.h"
#include "../../anim_water_ripple.h"
#include "../../gfx_2d_print.h"
#include "../../gfx_util.h"

using namespace std;

#define BUF_W 240
#define BUF_H 240

Graphics2DPrint gfx2d(BUF_W, BUF_H, 16);

// uint16_t maskColor = rgb565(0, 0, 0);

class RotationExampleWindow : public SDLWindowRGB565 {
 public:
  RotationExampleWindow(Graphics2D* gfx2d, int w, int h) : SDLWindowRGB565(gfx2d, w, h) {}
  void setup() {
    // gfx2d.enableMask(rgb565(0, 0, 0));
  }

  void loop() {
    static uint16_t counter = 1;
    Graphics2DPrint* gfx = &gfx2d;

    counter++;
    gfx->fill(rgb565(0, 0, 0));

    gfx->setTextSize(2);
    gfx->setTextCursor(220 - gfx->textCharXOffset(5), 42);
    gfx->print("Start");
    gfx->setTextCursor(220 - gfx->textCharXOffset(4), 182);
    gfx->print("Stop");

    gfx->setTextSize(4);
    gfx->setTextCursor(gfx->textCharXOffset(1), 120 - gfx->textCharYOffset(1) / 2);
    gfx->writeDigits(0, 1);
    gfx->print(":");
    gfx->writeDigits(11, 2);
    gfx->print(":");
    gfx->writeDigits(7, 3);

    // Draw watch outline
    gfx->drawCircle(119, 119, 119, rgb565(255, 255, 255));
    gfx->fillFrame(216, 40, 10, 10, rgb565(200, 200, 200));
    gfx->fillFrame(216, 190, 10, 10, rgb565(200, 200, 200));
    gfx->fillFrame(13, 40, 10, 10, rgb565(200, 200, 200));
    gfx->fillFrame(13, 190, 10, 10, rgb565(200, 200, 200));

    delay(1000 / 30);
  }
};

int main(int argc, char* argv[]) {
  RotationExampleWindow exampleWindow(&gfx2d, BUF_W, BUF_H);
  exampleWindow.run();
  return 0;
}
