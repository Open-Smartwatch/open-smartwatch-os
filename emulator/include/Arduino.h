#pragma once

#include "DataTypes.h"
#include "Defines.h"

unsigned long millis();
long random(int howbig);
long random(int howsmall, int howbig);
void delay(long millis);

// Following are just forwards to the their std-equivalents, specific implementations will confuse the compiler (and cause e.g. std::min != min != what-it-should-be problems)
template<typename T>
inline T min(T a, T b) {
    return std::min(a, b);
}
template<typename T>
inline T max(T a, T b) {
    return std::max(a, b);
}