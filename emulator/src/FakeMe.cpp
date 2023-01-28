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

esp_err_t esp_light_sleep_start() {
    OswEmulator::instance->enterSleep(false);
    throw OswEmulator::EmulatorSleep();
    return ESP_OK;
}

esp_err_t esp_sleep_enable_timer_wakeup(unsigned long time_in_us) {
    OSW_EMULATOR_THIS_IS_NOT_IMPLEMENTED;
    return ESP_FAIL;
}

esp_err_t esp_sleep_enable_ext0_wakeup(unsigned long gpio_num, int level) {
    OSW_EMULATOR_THIS_IS_NOT_IMPLEMENTED;
    return ESP_FAIL;
}

esp_err_t esp_sleep_enable_ext1_wakeup(unsigned long gpio_num, int level) {
    OSW_EMULATOR_THIS_IS_NOT_IMPLEMENTED;
    return ESP_FAIL;
}

esp_sleep_wakeup_cause_t esp_sleep_get_wakeup_cause() {
    OSW_EMULATOR_THIS_IS_NOT_IMPLEMENTED;
    return ESP_SLEEP_WAKEUP_UNDEFINED;
}