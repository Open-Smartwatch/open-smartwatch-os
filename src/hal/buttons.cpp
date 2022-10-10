#ifndef OSW_EMULATOR
#include "driver/rtc_io.h"
#endif
#include "osw_hal.h"
#include "osw_pins.h"

// assign pins to buttons
uint8_t buttonPins[] = {BTN_1, BTN_2, BTN_3};  // see osw_pins.h
uint8_t buttonClickStates[] = BTN_STATE_ARRAY;

// Graphics2D screenBuffer(DISP_W, DISP_H, DISP_CHUNK_H);

void OswHal::setupButtons(void) {
    // rtc_gpio_deinit(GPIO_NUM_0);
    // rtc_gpio_deinit(GPIO_NUM_10);
    // rtc_gpio_deinit(GPIO_NUM_13);
    pinMode(BTN_1, INPUT);
    pinMode(BTN_2, INPUT);
    pinMode(BTN_3, INPUT);
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
#ifndef NDEBUG
    Serial.print("Vibrate for: ");
    Serial.println(millis);
#endif
    delay(millis);
    digitalWrite(VIBRATE, LOW);
}
#endif

void OswHal::checkButtons(void) {
    // Buttons (Engine)
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        _btnIsDown[i] = digitalRead(buttonPins[i]) == buttonClickStates[i];
    }

    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
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
        _btnLongPress[i] = _btnIsDown[i] == true and (millis() > _btnIsDownMillis[i] + OswConfigAllKeys::appSwitcherLongPress.get());

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
    return _btnDoubleClick[btn];
}
bool OswHal::btnHasGoneUp(Button btn) {
    return _btnGoneUp[btn];
}
bool OswHal::btnIsDown(Button btn) {
    return _btnIsDown[btn];
}
bool OswHal::btnIsLongPress(Button btn) {
    return _btnLongPress[btn];
}
void OswHal::suppressButtonUntilUp(Button btn) {
    _btnSuppressUntilUpAgain[btn] = true;
}
unsigned long OswHal::btnIsDownSince(Button btn) {
    return _btnIsDown[btn] ? millis() - _btnIsDownMillis[btn] : 0;
}
void OswHal::clearButtonState(Button btn) {
    _btnGoneUp[btn] = false;
    _btnGoneDown[btn] = false;
    _btnLongPress[btn] = false;
}
