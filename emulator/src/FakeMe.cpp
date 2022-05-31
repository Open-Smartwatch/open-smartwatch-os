#include "../include/FakeMe.h"
#include "../include/Defines.h"

#include "Emulator.hpp"
#include "osw_hal.h"

void sleep(int) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
}

int rtc_get_reset_reason(int cpu) {
    return OswEmulator::instance->fromDeepSleep() ? 5 : 1;
}

void randomSeed(long l) {
    srand(l);
}

void esp_deep_sleep_start() {
    OswEmulator::instance->enterSleep(true);
    OswHal::resetInstance();
    throw OswEmulator::EmulatorSleep();
}

void esp_light_sleep_start() {
    OswEmulator::instance->enterSleep(false);
    OswHal::resetInstance();
    throw OswEmulator::EmulatorSleep();
}