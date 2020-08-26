#ifndef FAKE_ARDUINO_H
#define FAKE_ARDUINO_H
#include <SDL.h>
#include <stdint.h>
#include <stdlib.h>

#include <random>

std::mt19937_64 gen(std::random_device{}());

// copy over missing functions from Arduino.h here, and fix them so they run :)

long random(int howbig) {
  uint32_t x = gen();
  uint64_t m = uint64_t(x) * uint64_t(howbig);
  uint32_t l = uint32_t(m);
  if (l < howbig) {
    uint32_t t = -howbig;
    if (t >= howbig) {
      t -= howbig;
      if (t >= howbig) t %= howbig;
    }
    while (l < t) {
      x = rand();
      m = uint64_t(x) * uint64_t(howbig);
      l = uint32_t(m);
    }
  }
  return m >> 32;
}

long random(int howsmall, int howbig) {
  if (howsmall >= howbig) {
    return howsmall;
  }
  long diff = howbig - howsmall;
  return random(diff) + howsmall;
}

void delay(long millis) { SDL_Delay(millis); }

#endif