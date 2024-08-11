#include "Arduino.h"

#include <thread>
#include <chrono>
#include <stdint.h>
#include <stdlib.h>
#include <random>

std::mt19937_64 gen(std::random_device{}());

unsigned long millis() {
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

long random(int howbig) {
    uint32_t x = gen();
    uint64_t m = uint64_t(x) * uint64_t(howbig);
    int32_t l = int32_t(m);
    if (l < howbig) {
        int32_t t = -howbig;
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

void delay(long millis) {
    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}