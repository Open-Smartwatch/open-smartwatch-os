#ifndef P3DT_GFX_ANGLES_H
#define P3DT_GFX_ANGLES_H

// rotate a poitn around a center (cy,cy), with a radius r, 0 degrees ist 12 o'clock
float rpx(float cx, float x, float r) { return cx + x * cos((r - 90) * 1000.0 / 57296.0); }
float rpy(float cy, float y, float r) { return cy + y * sin((r - 90) * 1000.0 / 57296.0); }

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
#endif