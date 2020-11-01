#ifndef FAKE_ARDUINO_H
#define FAKE_ARDUINO_H

#include <stdint.h>
#include <math.h>

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

// copy over missing functions from Arduino.h here, and fix them so they run :)
long millis();
long random(int howbig);
long random(int howsmall, int howbig);
void delay(long millis);
int32_t min(int32_t a, int32_t b);
int32_t max(int32_t a, int32_t b);
#endif