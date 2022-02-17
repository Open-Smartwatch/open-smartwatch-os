#pragma once

#include "Defines.h"

class ESP_t {
public:
    void restart() {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
    }
};

extern ESP_t ESP;

#define ESP_OK true