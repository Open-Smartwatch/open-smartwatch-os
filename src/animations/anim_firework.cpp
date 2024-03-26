#include "animations/anim_firework.h"

#include "math_angles.h"
#include "gfx_util.h"
#include "gfx_2d.h"

void Particle::tick(long ms, float friction, float gravity) {
    // update position
    locationX += speedX * (ms / 1000.0f);
    locationY += speedY * (ms / 1000.0f);

    // update velocity
    speedX = speedX - friction * (ms / 1000.0f);
    speedY = speedY + gravity * (ms / 1000.0f);
    // printf("particle at %d/%d\n", locationX, locationY);
}

void Firework::init(uint16_t color_, uint8_t radius, uint8_t rings,  //
                    uint16_t screenWidth, uint16_t screenHeight) {
    height = 0;
    explHeight = random((float)screenHeight * 0.2f, (float)screenHeight * 0.8f);
    age = 0;
    color = color_;

    for (uint8_t i = 0; i < numParticles; i++) {
        // precalculate particle starting points
        float pointsOnRing = ((float)numParticles / (float)rings);
        uint8_t ring = (i / pointsOnRing) + 1;
        float angle = (360.0f / pointsOnRing) * i;

        particles[i].locationX = rpx(0, ring * radius, angle);
        particles[i].locationY = rpy(0, ring * radius, angle);

        // TODO: rotate particle velocities in a circle of radius
        particles[i].speedX = rpx(0, radius, angle) * 2;
        particles[i].speedY = rpy(0, radius, angle) * 2;
    }
}

void Firework::tick(long ms, uint8_t launchSpeed) {
    if (height < explHeight) {
        height += launchSpeed * (ms / 100.0f);
    } else {
        for (uint8_t i = 0; i < numParticles; i++) {
            particles[i].tick(ms, 0.1f, 9.8f);
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