
#include "./apps/_experiments/fireworks.h"

#include <anim_firework.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

// OswAppHelloWorld::OswAppHelloWorld(void) : OswApp() {}
#define NUM_FIREWORKS 10
Firework fireworks[NUM_FIREWORKS];
int16_t offsets[NUM_FIREWORKS];

void OswAppFireworks::setup() {
  OswHal::getInstance()->gfx()->fill(0);  // bg black
  for (uint8_t i = 0; i < NUM_FIREWORKS; i++) {
    offsets[i] = random(40, 200);
  }
}
void OswAppFireworks::loop() {
  static uint8_t countdown = 10;
  static long lastTick = millis();

  OswHal* hal = OswHal::getInstance();
  Graphics2D* gfx2d = hal->gfx();

  if (countdown == 0) {
    for (uint8_t i = 0; i < NUM_FIREWORKS; i++) {
      fireworks[i].draw(gfx2d, offsets[i], gfx2d->getHeight());
      fireworks[i].tick(millis() - lastTick, 10);

      if (fireworks[i].age > random(3000, 12000)) {
        uint16_t color = rgb565(random(100, 255), random(100, 255), random(100, 255));
        uint8_t radius = random(1, 8);
        uint8_t rings = random(3, 6);
        offsets[i] = random(4, 200);
        fireworks[i].init(color, radius, rings, gfx2d->getWidth(), gfx2d->getHeight());
      }
    }
    gfx2d->dim(10);
  } else {
    hal->getCanvas()->getGraphics2D()->fill(0);  // bg black
    hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
    hal->getCanvas()->setTextSize(3);
    hal->getCanvas()->setCursor(50, 110);
    hal->getCanvas()->print("23:59:");
    hal->getCanvas()->print(60 - countdown);
    countdown--;
    delay(1000);  // oops
    if (countdown == 0) {
      for (uint8_t i = 0; i < NUM_FIREWORKS; i++) {
        uint16_t color = rgb565(random(100, 255), random(100, 255), random(100, 255));
        uint8_t radius = random(1, 8);
        uint8_t rings = random(3, 6);
        offsets[i] = random(40, 200);
        fireworks[i].init(color, radius, rings, gfx2d->getWidth(), gfx2d->getHeight());
      }
    }
  }

  lastTick = millis();
  delay(1000 / 30);
}
