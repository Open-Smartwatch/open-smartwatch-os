#include "anim_firework.h"

void Particle::tick(long ms, float friction, float gravity) {
  // update position
  locationX += speedX * (ms / 1000.0);
  locationY += speedY * (ms / 1000.0);

  // update velocity
  speedX = speedX - friction * (ms / 1000.0);
  speedY = speedY + gravity * (ms / 1000.0);
  // printf("particle at %d/%d\n", locationX, locationY);
}

void Firework::init(uint16_t color_, uint8_t radius, uint8_t rings,  //
                    uint16_t screenWidth, uint16_t screenHeight) {
  height = 0;
  explHeight = random((float)screenHeight * 0.2, (float)screenHeight * 0.8);
  age = 0;
  color = color_;

  for (uint8_t i = 0; i < numParticles; i++) {
    // precalculate particle starting points
    float pointsOnRing = ((float)numParticles / (float)rings);
    uint8_t ring = (i / pointsOnRing) + 1;
    float angle = (360.0 / pointsOnRing) * i;

    particles[i].locationX = rpx(0, ring * radius, angle);
    particles[i].locationY = rpy(0, ring * radius, angle);

    // TODO: rotate particle velocities in a circle of radius
    particles[i].speedX = rpx(0, radius, angle) * 2;
    particles[i].speedY = rpy(0, radius, angle) * 2;
  }
}

void Firework::tick(long ms, uint8_t launchSpeed) {
  if (height < explHeight) {
    height += launchSpeed * (ms / 100.0);
  } else {
    for (uint8_t i = 0; i < numParticles; i++) {
      particles[i].tick(ms, .1, 9.8);
    }

    color = dimColor(color, age / 1000);
    age += ms;
  }
}

void Firework::draw(Graphics2D* gfx, int16_t offsetX, int16_t offsetY) {
  if (height < explHeight) {
    gfx->drawPixel(offsetX, offsetY - height, rgb565(255, 255, 255));
  } else {
    for (uint8_t i = 0; i < numParticles; i++) {
      gfx->drawPixel(offsetX + (int)particles[i].locationX, offsetY - height + (int)particles[i].locationY, color);
    }
  }
}