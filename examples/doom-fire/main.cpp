#include <FakeArduino.h>
#include <SDLWindow.h>
#include <stdint.h>

#include <iostream>

#include "../../src/anim/p3dt_anim_doom_fire.h"
#include "../../src/gfx/p3dt_gfx_util.h"
using namespace std;

#define BUF_W 240
#define BUF_H 120
uint8_t** firePixels = new uint8_t*[BUF_H];
Graphics2D gfx2d(BUF_W, BUF_H, 120);

class FireWindow : public SDLWindowRGB565 {
 public:
  FireWindow(Graphics2D* gfx2d, int w, int h) : SDLWindowRGB565(gfx2d, w, h) {}
  void setup() {
    // setup array rows
    for (int i = 0; i < BUF_H; i++) {
      firePixels[i] = new uint8_t[BUF_W];
    }
    setupFire(firePixels, BUF_W, BUF_H);
  }

  void loop() {
    delay(1000 / 30);
    calcFire(firePixels, BUF_W, BUF_H);
    mapFire(firePixels, BUF_W, BUF_H, &gfx2d, 0, 0);
  }
};

int main(int argc, char* argv[]) {
  FireWindow fw(&gfx2d, BUF_W, BUF_H);
  fw.run();
  return 0;
}
