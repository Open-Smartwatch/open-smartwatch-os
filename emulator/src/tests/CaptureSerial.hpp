#pragma once

#include <Serial.h>

class CaptureSerial {
  public:
    CaptureSerial() {
        Serial.setBuffered(true);
        Serial.buffer.clear(); // Tests tend to not like the remains of other runs
    }
    ~CaptureSerial() {
        Serial.setBuffered(false);
        // Clear is already done by the Serial class
    }
};