#pragma once

#include <Serial.h>

class CaptureSerial {
  public:
    CaptureSerial() {
        Serial.setBuffered(true);
    }
    ~CaptureSerial() {
        Serial.setBuffered(false);
    }
};