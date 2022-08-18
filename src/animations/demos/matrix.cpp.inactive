#include <stdint.h>
#include <stdio.h>

#include "../../FakeArduino.h"
#include "../../FakeArduinoWindowSDL.h"
#include "../../anim_matrix.h"
#include "../../anim_water_ripple.h"
#include "../../gfx_2d_print.h"
#include "../../gfx_util.h"

using namespace std;

#define BUF_W 240
#define BUF_H 240

Graphics2DPrint gfx2d(BUF_W, BUF_H, 16);

// create animation object
AnimMatrix m(&gfx2d);

class RotationExampleWindow : public SDLWindowRGB565 {
 public:
  RotationExampleWindow(Graphics2D* gfx2d, int w, int h) : SDLWindowRGB565(gfx2d, w, h) {}
  void setup() {}

  void loop() {
    static uint16_t counter = 1;
    Graphics2DPrint* gfx = &gfx2d;

    counter++;
    gfx->fill(rgb565(0, 0, 0));

    // render animation
    m.loop(gfx);

    delay(1000 / 30);
  }
};

int main(int argc, char* argv[]) {
  RotationExampleWindow exampleWindow(&gfx2d, BUF_W, BUF_H);
  exampleWindow.run();
  return 0;
}
