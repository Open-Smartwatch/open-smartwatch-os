#include <stdint.h>
#include <stdio.h>

#include "../../FakeArduino.h"
#include "../../FakeArduinoWindowSDL.h"
#include "../../gfx_2d_print.h"
#include "../../gfx_util.h"

#include "../../fonts/Picopixel.h"
#include "../../fonts/FreeSans11pt8b.h"

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

    gfx->setTextCenterAligned();
    gfx->setTextBottomAligned();
    gfx->setTextCursor(120, 150);
    gfx->setTextSize(2);
    gfx->print("Some specifics \n caracters \n \xA4 \xDF \xE0");  // does not support multiple rows

    gfx->setFont(&Picopixel);
    gfx->setTextCenterAligned();
    gfx->setTextCursor(120,200);
    gfx->setTextSize(1);
    gfx->print("Font pico Pixel");
    gfx->clearFont();

    gfx->setFont(&FreeSans11pt8b);
    gfx->setTextCenterAligned();
    gfx->setTextCursor(120,220);
    gfx->setTextSize(1);
    gfx->print("Font Serif Bold 9px");
    gfx->clearFont();


    /*
    gfx->fill(rgb565(0, 0, 0));

    int i = 0;
    int j = 0;
    gfx->setTextSize(1);
    for(i=0; i<11; i++){
      for(j=0;j<17;j++){
        gfx->setTextCursor(j*15+10,i*15+10);
        gfx->print(char(i*10+j+160));
      }
    }
    */

  }
};

int main(int argc, char* argv[]) {
  RotationExampleWindow exampleWindow(&gfx2d, BUF_W, BUF_H);
  exampleWindow.run();
  return 0;
}
