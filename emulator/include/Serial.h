#pragma once

#include <iostream>

#include "Defines.h"

class Serial_t {
public:
    Serial_t() {};
    ~Serial_t() {};

    template<typename T>
    void print(T smth) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        std::cout << smth << std::flush;
    }

    template<typename T>
    void println(T smth) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        std::cout << smth << std::endl;
    }

    void begin(int) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
    }
};

extern Serial_t Serial;