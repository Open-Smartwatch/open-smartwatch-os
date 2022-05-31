#include "../include/IO.h"
#include "../include/Defines.h"

#include "osw_hal.h"

#include "Emulator.hpp"
#include OSW_TARGET_PLATFORM_HEADER
#include "osw_pins.h" // Used for BTN_*

void pinMode(int pin, int mode) {
    if(pin == OSW_DEVICE_TPS2115A_STATPWR)
        return;
    else if(pin == OSW_DEVICE_ESP32_BATLVL)
        return;
    else if(pin == BTN_1)
        return;
    else if(pin == BTN_2)
        return;
    else if(pin == BTN_3)
        return;
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
}

uint8_t digitalRead(int pin) {
    const uint8_t buttonClickStates[] = BTN_STATE_ARRAY;
    if(pin == BTN_1)
        return ((OswEmulator::instance->getButton(0) ? LOW : HIGH) == buttonClickStates[0]) ? LOW : HIGH;
    else if(pin == BTN_2)
        return ((OswEmulator::instance->getButton(1) ? LOW : HIGH) == buttonClickStates[1]) ? LOW : HIGH;
    else if(pin == BTN_3)
        return ((OswEmulator::instance->getButton(2) ? LOW : HIGH) == buttonClickStates[2]) ? LOW : HIGH;
    else if(pin == OSW_DEVICE_TPS2115A_STATPWR)
        return OswEmulator::instance->isCharging() ? 1 : 0;
    else if(pin == TFT_LED)
        return 255; // The emulator has always full brightness for now...
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
    return LOW;
}

uint8_t analogRead(int pin) {
    if(pin == OSW_DEVICE_ESP32_BATLVL)
        return OswEmulator::instance->getBatteryRaw();
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
    return 0;
}

void digitalWrite(int, uint8_t) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
}

void ledcWrite(int, int) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
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