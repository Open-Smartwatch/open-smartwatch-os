#include "../include/IO.h"
#include "../include/Defines.h"

void pinMode(int, int) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
}

uint8_t digitalRead(int) {
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