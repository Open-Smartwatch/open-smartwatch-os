#include "math_angles.h"

#include <Arduino.h>

// seconds to degrees (0-360)
float s2d(long seconds) {
    return (seconds % 60) * 6;
}

// minutes to degrees (0-360)
float m2d(long seconds) {
    long fh = (seconds / 3600);  // full hours
    return (((seconds - fh * 3600) / 60.0f)) * 6;
}

// hours to degrees (0-360)
float h2d(long seconds) {
    long fd = (seconds / 3600) / 24;  // full days
    return ((seconds - fd * 24 * 3600) / 3600.0f) * 30;
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