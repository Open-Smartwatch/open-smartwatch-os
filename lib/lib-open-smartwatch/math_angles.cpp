#include "math_angles.h"

#ifdef OSW_EMULATOR
#include "FakeArduino.h"
#else
#include <Arduino.h>
#endif

// rotate a point around a center (cy,cy), with a radius r, 0 degrees ist 12 o'clock
float rpx(float cx, float r, float d) { return cx + r * cos((d - 90) * 1000.0 / 57296.0); }
float rpy(float cy, float r, float d) { return cy + r * sin((d - 90) * 1000.0 / 57296.0); }

// rotate a point around a point
int32_t rotateX(int32_t x, int32_t y, int32_t rx, int32_t ry, float cosA, float sinA) {
  return (x - rx) * cosA + (y - ry) * sinA;
}
int32_t rotateY(int32_t x, int32_t y, int32_t rx, int32_t ry, float cosA, float sinA) {
  return (y - ry) * cosA - (x - rx) * sinA;
}

int32_t rotateX(int32_t x, int32_t y, int32_t rx, int32_t ry, float a) {
  return (x - rx) * cos(a) + (y - ry) * sin(a);
}
int32_t rotateY(int32_t x, int32_t y, int32_t rx, int32_t ry, float a) {
  return (y - ry) * cos(a) - (x - rx) * sin(a);
}


// seconds to degrees (0-360)
float s2d(long seconds) { return (seconds % 60) * 6; }

// minutes to degrees (0-360)
float m2d(long seconds) {
  long fh = (seconds / 3600);  // full hours
  return (((seconds - fh * 3600) / 60.0)) * 6;
}

// hours to degrees (0-360)
float h2d(long seconds) {
  long fd = (seconds / 3600) / 24;  // full days
  return ((seconds - fd * 24 * 3600) / 3600.0) * 30;
}

float sign(float x1, float y1, float x2, float y2, float x3, float y3) {
  return (x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3);
}

// Source: https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle
bool pointInsideTriangle(float px, float py, float x1, float y1, float x2, float y2, float x3, float y3) {
  float d1, d2, d3;
  bool has_neg, has_pos;

  d1 = sign(px, py, x1, y1, x2, y2);
  d2 = sign(px, py, x2, y2, x3, y3);
  d3 = sign(px, py, x3, y3, x1, y1);

  has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
  has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

  return !(has_neg && has_pos);
}