#include "animations/anim_doom_fire.h"

AnimDoomFire::AnimDoomFire() {
    firePixels = new uint8_t* [240];
    for (int i = 0; i < 240; i++)
        firePixels[i] = new uint8_t[240];
    setupFire(firePixels, 240, 240);
}

AnimDoomFire::~AnimDoomFire() {
    for (int i = 0; i < 240; i++)
        delete[] firePixels[i];
    delete[] firePixels;
}

void AnimDoomFire::setupFire(uint8_t** firePixels, uint16_t w, uint16_t h) {
    for (uint8_t y = 0; y < h; y++) {
        for (uint8_t x = 0; x < w; x++) {
            // last row is hot
            firePixels[y][x] = y == h - 1 ? 35 : 0;
        }
    }
}

void AnimDoomFire::calcFire(uint8_t** firePixels, uint16_t w, uint16_t h,float* X,float* Y) {
    for (uint8_t y = 0; y < h - 1; y++) {
        for (uint8_t x = 0; x < w; x++) {
            uint8_t wind = x + random(2) + (Y == nullptr ? 0 : (abs(*Y) / 15000));
            wind = wind >= w ? wind - w : wind;
            uint8_t speed = y + random(2) + (X == nullptr ? 0:(abs(*X) / 15000));
            speed = speed >= h ? h - 1 : speed;
            firePixels[y][x] = firePixels[speed][wind] - random(2);
            firePixels[y][x] = firePixels[y][x] > 35 ? 0 : firePixels[y][x];  // fix overflow
        }
    }
}

void AnimDoomFire::mapFire(uint8_t** firePixels, uint16_t fireW, uint16_t fireH, Graphics2D* graphics2d, uint16_t offsetX, uint16_t offsetY) {
    for (uint8_t x = 0; x < fireW; x++) {
        for (uint8_t y = 0; y < fireH; y++) {
            graphics2d->drawPixel(x + offsetX, y + offsetY, doomColorMap[firePixels[y][x]]);
        }
    }
}

void AnimDoomFire::loop(Graphics2DPrint* gfx,float* X, float* Y) {
    calcFire(firePixels, 240, 240,X,Y);
    mapFire(firePixels, 240, 240, gfx, 0, 0);
}
