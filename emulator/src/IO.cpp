#include "../include/IO.h"
#include "../include/Defines.h"

#include "Emulator.hpp"
#include "osw_pins.h" // Used for BTN_*

void pinMode(int, int) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
}

uint8_t digitalRead(int pin) {
    if(pin == BTN_1)
        return OswEmulator::instance->getButton(0) ? LOW : HIGH;
    else if(pin == BTN_2)
        return OswEmulator::instance->getButton(1) ? LOW : HIGH;
    else if(pin == BTN_3)
        return OswEmulator::instance->getButton(2) ? LOW : HIGH;
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
    return LOW;
}

uint8_t analogRead(int) {
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
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
}

void esp_light_sleep_start() {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
}