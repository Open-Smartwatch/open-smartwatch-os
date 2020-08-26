#include <FakeArduino.h>
#include <SDLWindow.h>
#include <stdint.h>

#include <iostream>

#include "../../anim/p3dt_anim_doom_fire.h"
#include "../../gfx/p3dt_gfx_util.h"
using namespace std;

#define BUF_W 240
#define BUF_H 120
uint16_t screenBuffer[BUF_W * BUF_H];
uint8_t** firePixels = new uint8_t*[BUF_H];

class FireWindow : public SDLWindowRGB565 {
 public:
  FireWindow(uint16_t* b, int w, int h) : SDLWindowRGB565(b, w, h) {}
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
    mapFire(firePixels, screenBuffer, BUF_W, BUF_H);
  }
};

int main(int argc, char* argv[]) {
  FireWindow fw(screenBuffer, BUF_W, BUF_H);
  fw.run();
  return 0;
}
