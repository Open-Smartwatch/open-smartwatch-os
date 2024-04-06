#ifndef OSW_EMULATOR
#include "driver/rtc_io.h"
#endif
#include "osw_hal.h"
#include "osw_pins.h"

const char* ButtonNames[BTN_NUMBER] = BTN_NAME_ARRAY;
static uint8_t buttonPins[BTN_NUMBER] = BTN_PIN_ARRAY;
static uint8_t buttonClickStates[BTN_NUMBER] = BTN_STATE_ARRAY;
static int16_t buttonPositionsX[BTN_NUMBER] = BTN_POSX_ARRAY;
static int16_t buttonPositionsY[BTN_NUMBER] = BTN_POSY_ARRAY;
static bool buttonIsTop[BTN_NUMBER] = BTN_POS_ISTOP_ARRAY;
static bool buttonIsLeft[BTN_NUMBER] = BTN_POS_ISLEFT_ARRAY;

#ifndef OSW_EMULATOR
void IRAM_ATTR ISR_BTN1() {
    setCpuFrequencyMhz(OSW_PLATFORM_DEFAULT_CPUFREQ);
}

void IRAM_ATTR ISR_BTN2() {
    setCpuFrequencyMhz(OSW_PLATFORM_DEFAULT_CPUFREQ);
}

void IRAM_ATTR ISR_BTN3() {
    setCpuFrequencyMhz(OSW_PLATFORM_DEFAULT_CPUFREQ);
}
#endif

void OswHal::setupButtons(void) {
    // rtc_gpio_deinit(GPIO_NUM_0);
    // rtc_gpio_deinit(GPIO_NUM_10);
    // rtc_gpio_deinit(GPIO_NUM_13);
    pinMode(BTN_1, INPUT);
    pinMode(BTN_2, INPUT);
    pinMode(BTN_3, INPUT);

#ifndef OSW_EMULATOR
    // raise speed to maximum if a button is pressed
    attachInterrupt(BTN_1, ISR_BTN1, CHANGE);
    attachInterrupt(BTN_2, ISR_BTN2, CHANGE);
    attachInterrupt(BTN_3, ISR_BTN3, CHANGE);
#endif

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)

    pinMode(VIBRATE, OUTPUT);
#endif

    // Buttons (Engine)
    for (uint8_t i = 0; i < 3; i++) {
        _btnLastState[i] = false;
        _btnIsDown[i] = false;
        _btnGoneUp[i] = false;
        _btnDoubleClick[i] = false;
        _btnDetectDoubleClickCount[i] = 0;
    }
}

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)

void OswHal::vibrate(long millis) {
    digitalWrite(VIBRATE, HIGH);
    OSW_LOG_D("Vibrate for: ", millis);
    delay(millis);
    digitalWrite(VIBRATE, LOW);
}
#endif

void OswHal::checkButtons(void) {
    // Buttons (Engine)
    for (uint8_t i = 0; i < BTN_NUMBER; i++) {
        _btnIsDown[i] = digitalRead(buttonPins[i]) == buttonClickStates[i];
        if(_btnIsDown[i])
            this->noteUserInteraction(); // Button pressing counts as user interaction
    }

    for (uint8_t i = 0; i < BTN_NUMBER; i++) {
        _btnGoneUp[i] = _btnLastState[i] == true && _btnIsDown[i] == false;
        _btnGoneDown[i] = _btnLastState[i] == false && _btnIsDown[i] == true;
        _btnDoubleClick[i] = false;
        // store the time stamp since the button went down
        if (_btnGoneDown[i]) {
            _btnIsDownMillis[i] = millis();
            if(_btnDetectDoubleClickCount[i]==0) {
                _btnDoubleClickMillis[i] = millis();
            }
            _btnDetectDoubleClickCount[i] = _btnDetectDoubleClickCount[i] + 1;
            _btnDoubleClick[i] = 2 == _btnDetectDoubleClickCount[i];
        }
        if(millis() - _btnDoubleClickMillis[i] > 500) {
            _btnDetectDoubleClickCount[i] = 0;
        }

        // check if the button has been down long enough
        _btnLongPress[i] = _btnIsDown[i] == true and (millis() > _btnIsDownMillis[i] + OswConfigAllKeys::oswAppV2ButtonLongPress.get());

        // store current button state
        _btnLastState[i] = _btnIsDown[i];

        // ignore all changes until up
        if (_btnSuppressUntilUpAgain[i] == true && _btnIsDown[i]) {
            _btnGoneUp[i] = false;
            _btnGoneDown[i] = false;
            _btnIsDownMillis[i] = millis();
            _btnLongPress[i] = false;
            _btnIsDown[i] = false;

            _btnDetectDoubleClickCount[i] = 0;
            _btnDoubleClickMillis[i] = millis();
            _btnDoubleClick[i] = false;
        } else {
            _btnSuppressUntilUpAgain[i] = false;
        }
    }
}

// Buttons (Engine)
bool OswHal::btnHasGoneDown(Button btn) {
    return _btnGoneDown[btn];
}
bool OswHal::btnIsDoubleClick(Button btn) {
    OSW_LOG_W("Deprecated method called. Please use OswAppV2::onButton() instead.");
    return _btnDoubleClick[btn];
}
bool OswHal::btnHasGoneUp(Button btn) {
    return _btnGoneUp[btn];
}
bool OswHal::btnIsDown(Button btn) {
    return _btnIsDown[btn];
}
bool OswHal::btnIsLongPress(Button btn) {
    OSW_LOG_W("Deprecated method called. Please use OswAppV2::onButton() instead.");
    return _btnLongPress[btn];
}
void OswHal::suppressButtonUntilUp(Button btn) {
    OSW_LOG_W("Deprecated method called. Please use OswAppV2::onButton() instead.");
    _btnSuppressUntilUpAgain[btn] = true;
}
unsigned long OswHal::btnIsDownFor(Button btn) {
    return _btnIsDown[btn] ? millis() - _btnIsDownMillis[btn] : 0;
}
unsigned long OswHal::btnIsDownSince(Button btn) {
    return _btnIsDown[btn] ? _btnIsDownMillis[btn] : 0;
}
void OswHal::clearButtonState(Button btn) {
    _btnGoneUp[btn] = false;
    _btnGoneDown[btn] = false;
    _btnLongPress[btn] = false;
}
bool OswHal::btnIsTopAligned(Button btn) {
    return buttonIsTop[btn];
}
bool OswHal::btnIsLeftAligned(Button btn) {
    return buttonIsLeft[btn];
}
void OswHal::getButtonCoordinates(Button btn, int16_t& x, int16_t& y) {
    x = buttonPositionsX[btn];
    y = buttonPositionsY[btn];
}
