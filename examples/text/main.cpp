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

    gfx->setTextSize(1);
    gfx->setTextLeftAligned();
    gfx->setTextCursor(120, 20);
    gfx->print("Left\naligned");
    gfx->setTextRightAligned();
    gfx->setTextCursor(120, 40);
    gfx->print("Right\naligned");

    gfx->setTextCursor(0, 120);
    gfx->setTextLeftAligned();
    gfx->setTextMiddleAligned();
    gfx->print("middle1\nmiddle2");

    gfx->setTextCenterAligned();
    gfx->setTextTopAligned();
    gfx->setTextCursor(120, 120);
    gfx->print("Top aligned\n(top supports multiple_rows)");

    gfx->setTextCenterAligned();
    gfx->setTextBottomAligned();
    gfx->setTextCursor(120, 120);
    gfx->print("Bottom aligned");  // does not support multiple rows

    gfx->setTextLeftAligned();
    gfx->setTextSize(2);
    gfx->setTextCursor(120, 200);
    // TODO: fix text wrapping on single character prints
    // gfx->setTextWrap(false);;
    gfx->setTextCursor(120 - gfx->getTextOfsetColumns(6.5), 210);
    gfx->printDecimal(8, 2);
    gfx->print(":");
    gfx->printDecimal(15, 2);
    gfx->print(":");
    gfx->printDecimal(47, 2);
    gfx->print(".");
    gfx->printDecimal(11, 4);

    delay(1000 / 30);
  }
};

int main(int argc, char* argv[]) {
  RotationExampleWindow exampleWindow(&gfx2d, BUF_W, BUF_H);
  exampleWindow.run();
  return 0;
}
