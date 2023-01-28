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

enum esp_err_t: int {
  ESP_FAIL = -1,
  ESP_OK = 0
  // Add more as needed...
};