#pragma once

#include <iostream>

#include "Defines.h"

class Serial_t {
public:
    Serial_t() {};
    ~Serial_t() {};

    template<typename T>
    void print(T smth) {
        std::cout << smth << std::flush;
    }

    template<typename T>
    void println(T smth) {
        std::cout << smth << std::endl;
    }

    void begin(int b) {
        std::cout << "Serial initialized with " << b << " bauds." << std::endl;
    }
};

extern Serial_t Serial;