#include "../include/FakeMe.h"
#include "../include/Defines.h"

void sleep(int) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
}

int rtc_get_reset_reason(int) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
    return 0;
}

void randomSeed(long l) {
    FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
    srand(l);
}