#pragma once

#include "time.h"

#include "Defines.h"

class RtcDateTime {
public:
    void InitWithEpoch32Time(time_t) {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
    }

    uint32_t Hour() {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    }
    uint32_t Minute() {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    }
    uint32_t Second() {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0;
    }

    uint32_t DayOfWeek() {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0; 
    };
    uint32_t Day() {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0; 
    };
    uint32_t Month() {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0; 
    };
    uint32_t Year() {
        FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
        return 0; 
    };
};