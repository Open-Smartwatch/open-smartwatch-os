
#include <stdint.h>

#include <iostream>

#include "../../FakeArduino.h"
#include "../../FakeArduinoWindowSDL.h"
#include "../../anim_doom_fire.h"
#include "../../gfx_util.h"
using namespace std;

#define BUF_W 240
#define BUF_H 240

Graphics2DPrint gfx2d(BUF_W, BUF_H, 120);
AnimDoomFire d;
class FireWindow : public SDLWindowRGB565 {
 public:
  FireWindow(Graphics2D* gfx2d, int w, int h) : SDLWindowRGB565(gfx2d, w, h) {}
  void setup() {

  }

  void loop() {
    Graphics2DPrint* gfx = &gfx2d;

    gfx->fill(rgb565(0, 0, 0));

    // render animation
    d.loop(gfx);

    delay(1000 / 30);
  }
};

int main(int argc, char* argv[]) {
  FireWindow fw(&gfx2d, BUF_W, BUF_H);
  fw.run();
  return 0;
}
