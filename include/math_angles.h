#ifndef GFX_ANGLES_H
#define GFX_ANGLES_H

#include<stdio.h>
#include <Arduino.h>

extern const float sinDeg[];

/**
 * @brief Find the x-axis point which is at a distance r and an angle d of a point C(cx,cy).
 *
 * 0 degrees is 12 o'clock
 *
 * This function can be used to find coordinates of the extremity of the clock hand from the center
 *
 * @param cx x value of the initial point
 * @param r radius
 * @param deg angle in degrees (0° is 12 o'clock)
 * @return int32_t
 */
inline int32_t rpx(int32_t cx, int32_t r, float deg) {
    return cx + (float) r * cosf((deg - 90) * (float) PI / 180) + 0.5f;
}

// integer version for deg
inline int32_t rpx(int32_t cx, int32_t r, int32_t deg) {
    deg -= 90;
    deg -= 90; // to use sine
    while (deg < 0)
        deg += 360;
    while (deg > 360)
        deg -= 360;
    return cx + (float) r * sinDeg[deg] + 0.5f;
}

/**
 * Find the y-axis of a point which is at a distance r and an angle d of a point C(cx,cy).
 *
 *      0 degrees is 12 o'clock
 *      This function can be used to find coordinates of the extremity of the clock hand from the center
 *
 * @param cy y value of the initial point
 * @param r radius
 * @param deg angle in degrees (0° is 12 o'clock)
 * @return float
 */
inline int32_t rpy(int32_t cy, int32_t r, float deg) {
    return cy + (float) r * sinf((deg - 90) * (float) PI / 180) + 0.5f;
}

// integer version for deg
inline int32_t rpy(int32_t cy, int32_t r, int32_t deg) {
    deg -= 90;
    while (deg < 0)
        deg += 360;
    while (deg > 360)
        deg -= 360;
    return cy + (float) r * sinDeg[deg] + 0.5f;
}

// rotate a point around a point
inline int32_t rotateX(int32_t x, int32_t y, int32_t rx, int32_t ry, float cosA, float sinA) {
    return (x - rx) * cosA + (y - ry) * sinA;
}
inline int32_t rotateY(int32_t x, int32_t y, int32_t rx, int32_t ry, float cosA, float sinA) {
    return (y - ry) * cosA - (x - rx) * sinA;
}

inline int32_t rotateX(int32_t x, int32_t y, int32_t rx, int32_t ry, float a) {
    return (x - rx) * cosf(a) + (y - ry) * sinf(a);
}
inline int32_t rotateY(int32_t x, int32_t y, int32_t rx, int32_t ry, float a) {
    return (y - ry) * cosf(a) - (x - rx) * sinf(a);
}

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
