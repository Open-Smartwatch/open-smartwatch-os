#pragma once

#include "Arduino_GFX.h"
#include "Defines.h"

class FakeDisplay : public Arduino_G {
public:
    FakeDisplay() : Arduino_G(128, 128, 32) {};
    ~FakeDisplay() {};

    void displayOn() {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
    };
    void displayOff() {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
    };
};