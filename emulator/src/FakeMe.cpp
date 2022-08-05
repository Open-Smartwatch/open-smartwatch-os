#include "../include/FakeMe.h"
#include "../include/Defines.h"

#include "Emulator.hpp"
#include "osw_hal.h"

#include <thread>
#include <chrono>

void sleep(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
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
    throw OswEmulator::EmulatorSleep();
}