#ifndef GFX_ANGLES_H
#define GFX_ANGLES_H

#ifdef FAKE_ARDUINO
#include "FakeArduino.h"
#else
#include <Arduino.h>
#endif

// rotate a point around a center (cy,cy), with a radius r, 0 degrees ist 12 o'clock
float rpx(float cx, float x, float r);
float rpy(float cy, float y, float r);


int32_t rotateX(int32_t x, int32_t y, int32_t rx, int32_t ry, float angle);
int32_t rotateY(int32_t x, int32_t y, int32_t rx, int32_t ry, float angle);

// seconds to degrees (0-360)
float s2d(long seconds);

// minutes to degrees (0-360)
float m2d(long seconds);

// hours to degrees (0-360)
float h2d(long seconds);
#endif