#ifndef P3DT_ANIM_DOOM_FIRE_H
#define P3DT_ANIM_DOOM_FIRE_H

#ifdef FAKE_ARDUINO
#include "FakeArduino.h"
#else
#include <Arduino.h>
#endif

#include "gfx_2d.h"
#include "gfx_util.h"
// see: https://p3dt.net/post/2019/01/05/playing-with-doom.html


void setupFire(uint8_t **firePixels, uint16_t w, uint16_t h);

void calcFire(uint8_t **firePixels, uint16_t w, uint16_t h);

void mapFire(uint8_t **firePixels, uint16_t fireW, uint16_t fireH,  //
             Graphics2D *graphics2d, uint16_t offsetX, uint16_t offsetY);

#endif