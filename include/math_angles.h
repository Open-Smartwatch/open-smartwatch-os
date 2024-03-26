#ifndef GFX_ANGLES_H
#define GFX_ANGLES_H

#include <Arduino.h>

/**
 * @brief Find the x-axis point which is at a distance r and an angle d of a point C(cx,cy).
 *
 * 0 degrees ist 12 o'clock
 *
 * This function can be used to find coordonnates of the extremity of the clock hand from the center
 *
 * @param cx x value of the initial point
 * @param r radius
 * @param d angle in degrees (0° is 12 o'clock)
 * @return float
 */
inline int32_t rpx(float cx, float r, float d) {
    return cx + (float) r * cosf((d - 90) * (float) PI / 180) + 0.5f;
}

/**
 * Find the y-axis of a point which is at a distance r and an angle d of a point C(cx,cy).
 *
 *      0 degrees ist 12 o'clock
 *      This function can be used to find coordonnates of the extremity of the clock hand from the center
 *
 * @param cy y value of the initial point
 * @param r radius
 * @param d angle in degrees (0° is 12 o'clock)
 * @return float
 */
inline int32_t rpy(float cy, float r, float d) {
    return cy + (float) r * sinf((d - 90) * (float) PI / 180) + 0.5f;
}

int32_t rotateX(int32_t x, int32_t y, int32_t rx, int32_t ry, float cosA, float sinA);
int32_t rotateY(int32_t x, int32_t y, int32_t rx, int32_t ry, float cosA, float sinA);
int32_t rotateX(int32_t x, int32_t y, int32_t rx, int32_t ry, float a);
int32_t rotateY(int32_t x, int32_t y, int32_t rx, int32_t ry, float a);

/**
 * Convert seconds in degrees.
 *
 * 0 seconds = 0° / 15 seconds = 90° ...
 *
 * @param seconds seconds to convert
 * @return float angle in degrees
 */
float s2d(long seconds);

// minutes to degrees (0-360)
float m2d(long seconds);

// hours to degrees (0-360)
float h2d(long seconds);

bool pointInsideTriangle(float px, float py, float x1, float y1, float x2, float y2, float x3, float y3);
#endif
