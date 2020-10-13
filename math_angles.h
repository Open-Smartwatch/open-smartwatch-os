#ifndef GFX_ANGLES_H
#define GFX_ANGLES_H

#ifdef FAKE_ARDUINO
#include "FakeArduino.h"
#else
#include <Arduino.h>
#endif

// rotate a poitn around a center (cy,cy), with a radius r, 0 degrees ist 12 o'clock
float rpx(float cx, float x, float r);
float rpy(float cy, float y, float r);

// seconds to degrees (0-360)
float s2d(long seconds);

// minutes to degrees (0-360)
float m2d(long seconds);

// hours to degrees (0-360)
float h2d(long seconds);
#endif