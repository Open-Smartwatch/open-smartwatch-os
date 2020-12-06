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

int32_t rotateX(int32_t x, int32_t y, int32_t rx, int32_t ry, float cosA, float sinA);
int32_t rotateY(int32_t x, int32_t y, int32_t rx, int32_t ry, float cosA, float sinA);
int32_t rotateX(int32_t x, int32_t y, int32_t rx, int32_t ry, float a);
int32_t rotateY(int32_t x, int32_t y, int32_t rx, int32_t ry, float a);
// seconds to degrees (0-360)
float s2d(long seconds);

// minutes to degrees (0-360)
float m2d(long seconds);

// hours to degrees (0-360)
float h2d(long seconds);

bool pointInsideTriangle(float px, float py, float x1, float y1, float x2, float y2, float x3, float y3);
#endif