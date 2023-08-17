#include "../include/IO.h"
#include "../include/Defines.h"
#include <OswLogger.h>

#include "Emulator.hpp"
#include OSW_TARGET_PLATFORM_HEADER
#include "hal/buttons.h"
#include "osw_pins.h" // Used for BTN_*

void pinMode(int pin, int mode) {
    switch (pin) {
    case OSW_DEVICE_TPS2115A_STATPWR:
    case OSW_DEVICE_ESP32_BATLVL:
    case BTN_1:
    case BTN_2:
    case BTN_3:
        return;
        break;
    default:
        OSW_EMULATOR_THIS_IS_NOT_IMPLEMENTED;
    }
}

uint8_t digitalRead(int pin) {
    const uint8_t buttonClickStates[] = BTN_STATE_ARRAY;
    switch (pin) {
    case BTN_1:
        return ((OswEmulator::instance->getButton(Button::BUTTON_SELECT) ? LOW : HIGH) == buttonClickStates[Button::BUTTON_SELECT]) ? LOW : HIGH;
        break;
    case BTN_2:
        return ((OswEmulator::instance->getButton(Button::BUTTON_UP) ? LOW : HIGH) == buttonClickStates[Button::BUTTON_UP]) ? LOW : HIGH;
        break;
    case BTN_3:
        return ((OswEmulator::instance->getButton(Button::BUTTON_DOWN) ? LOW : HIGH) == buttonClickStates[Button::BUTTON_DOWN]) ? LOW : HIGH;
        break;
    case OSW_DEVICE_TPS2115A_STATPWR:
        return OswEmulator::instance->isCharging() ? 1 : 0;
        break;
    default:
        OSW_EMULATOR_THIS_IS_NOT_IMPLEMENTED;
        return LOW;
    }
}

uint8_t analogRead(int pin) {
    if(pin == OSW_DEVICE_ESP32_BATLVL)
        return OswEmulator::instance->getBatteryRaw();
    OSW_EMULATOR_THIS_IS_NOT_IMPLEMENTED;
    return 0;
}

void digitalWrite(int, uint8_t) {
    OSW_EMULATOR_THIS_IS_NOT_IMPLEMENTED;
}

void ledcWrite(int, int) {
    OSW_EMULATOR_THIS_IS_NOT_IMPLEMENTED;
}