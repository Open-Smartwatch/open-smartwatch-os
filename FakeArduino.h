#ifndef FAKE_ARDUINO_H
#define FAKE_ARDUINO_H

#include <stdint.h>

// copy over missing functions from Arduino.h here, and fix them so they run :)
long millis();
long random(int howbig);
long random(int howsmall, int howbig);
void delay(long millis);
#endif