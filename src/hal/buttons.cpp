
#include "driver/rtc_io.h"
#include "osw_hal.h"
#include "osw_pins.h"

// assign pins to buttons
uint8_t buttonPins[] = {BTN_1, BTN_2, BTN_3};  // see osw_pins.h
// assign active LOW or HIGH states according to hardware
#if defined(GPS_EDITION_ROTATED)
uint8_t buttonClickStates[] = {HIGH, HIGH, LOW};
#else
uint8_t buttonClickStates[] = {LOW, HIGH, HIGH};
#endif

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

    // store the time stamp since the button went down
    if (_btnGoneDown[i]) {
      _btnIsDownMillis[i] = millis();
    }

    // check if the button hass been down long enough
    _btnLongPress[i] = millis() > _btnIsDownMillis[i] + DEFAULTLAUNCHER_LONG_PRESS && _btnIsDown[i] == true;

    // store current button state
    _btnLastState[i] = _btnIsDown[i];

    // ignore all changes until up
    if (_btnSuppressUntilUpAgain[i] == true && _btnIsDown[i]) {
      _btnGoneUp[i] = false;
      _btnGoneDown[i] = false;
      _btnIsDownMillis[i] = millis();
      _btnLongPress[i] = false;
      _btnIsDown[i] = false;
    } else {
      _btnSuppressUntilUpAgain[i] = false;
    }
  }
}

// Buttons (Engine)
bool OswHal::btnHasGoneDown(Button btn) { return _btnGoneDown[btn]; }
bool OswHal::btnHasGoneUp(Button btn) { return _btnGoneUp[btn]; }
bool OswHal::btnIsDown(Button btn) { return _btnIsDown[btn]; }
bool OswHal::btnIsLongPress(Button btn) { return _btnLongPress[btn]; }
void OswHal::suppressButtonUntilUp(Button btn) { _btnSuppressUntilUpAgain[btn] = true; }
unsigned long OswHal::btnIsDownSince(Button btn) { return _btnIsDown[btn] ? millis() - _btnIsDownMillis[btn] : 0; }
void OswHal::clearButtonState(Button btn) {
  _btnGoneUp[btn] = false;
  _btnGoneDown[btn] = false;
  _btnLongPress[btn] = false;
}
