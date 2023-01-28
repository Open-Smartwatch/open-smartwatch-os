#pragma once

#include "Emulator.hpp"
#include "Defines.h"

class ESP_t {
  public:
    void restart() {
        OswEmulator::instance->enterSleep(true);
        throw OswEmulator::EmulatorSleep();
    }
};

extern ESP_t ESP;