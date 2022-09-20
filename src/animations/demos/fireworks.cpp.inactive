#include <stdint.h>

#include <iostream>

#include "../../FakeArduino.h"
#include "../../FakeArduinoWindowSDL.h"
#include "../../anim_firework.h"
#include "../../gfx_2d.h"
#include "../../gfx_util.h"
#include "../../math_angles.h"
using namespace std;

#define BUF_W 240
#define BUF_H 240

#define NUM_FIREWORKS 10
Graphics2D gfx2d(BUF_W, BUF_H, 4, false);
Firework fireworks[NUM_FIREWORKS];
int16_t offsets[NUM_FIREWORKS];

class WatchSimpleWindow : public SDLWindowRGB565 {
 public:
  WatchSimpleWindow(Graphics2D* gfx2d, int w, int h) : SDLWindowRGB565(gfx2d, w, h) {}
  void setup() {
    // draw background black
    gfx2d.fill(rgb565(0, 0, 0));
    for (uint16_t i = 0; i < NUM_FIREWORKS; i++) {
      offsets[i] = random(40, 200);
    }
  }

  void loop() {
    static long lastTick = millis();

    for (uint16_t i = 0; i < NUM_FIREWORKS; i++) {
      fireworks[i].draw(&gfx2d, offsets[i], BUF_H);
      fireworks[i].tick(millis() - lastTick, 10);

      if (fireworks[i].age > random(3000, 12000)) {
        uint16_t color = rgb565(random(100, 255), random(100, 255), random(100, 255));
        uint8_t radius = random(1, 8);
        uint8_t rings = random(3, 6);
        offsets[i] = random(40, 200);
        fireworks[i].init(color, radius, rings, BUF_W, BUF_H);
      }
    }

    gfx2d.dim(10);

    lastTick = millis();
    delay(1000 / 30);
  }
};

int main(int argc, char* argv[]) {
  WatchSimpleWindow wsw(&gfx2d, BUF_W, BUF_H);
  wsw.run();
  return 0;
}
