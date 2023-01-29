#pragma once

#include <Serial.h>

class CaptureSerialFixture {
  public:
    CaptureSerialFixture() {
        Serial.setBuffered(true);
        Serial.begin(115200);
        Serial.buffer.clear(); // Tests tend to not like the remains of other runs
    }
    ~CaptureSerialFixture() {
        Serial.setBuffered(false);
        // Clear is already done by the Serial class
    }
};