#ifndef TIME_UTIL_H
#define TIME_UTIL_H

#ifdef FAKE_ARDUINO
#include "FakeArduino.h"
#else
#include <Arduino.h>
#endif

long seconds(long seconds);
long minutes(long seconds);
long hours24(long seconds);

#endif