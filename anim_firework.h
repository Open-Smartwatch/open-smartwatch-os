#ifndef ANIM_FIREWORK_H
#define ANIM_FIREWORK_H

#ifdef FAKE_ARDUINO
#include "FakeArduino.h"
#else
#include <Arduino.h>
#endif

#include "gfx_2d.h"
#include "gfx_util.h"

class Particle {
 public:
  Particle() {}
  float locationX;
  float locationY;
  float speedX;
  float speedY;
  void tick(long ms, float friction, float gravity);
};

class Firework {
 private:
  Particle* particles;
  uint8_t numParticles;

 public:
  Firework() {
    numParticles = 53;
    particles = new Particle[numParticles];
  }
  ~Firework(void) { delete[] particles; }

  void init(uint16_t color, uint8_t radius, uint8_t rings,  //
            uint16_t screenWidth, uint16_t screenHeight);
  void tick(long ms, uint8_t launchSpeed);

  void draw(Graphics2D* gfx, int16_t offsetX, int16_t offsetY);
  uint16_t height;
  uint16_t explHeight;
  uint16_t color;
  long age;
};
#endif