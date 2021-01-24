
#include "osw_hal.h"
#include "osw_pins.h"

// Graphics2D screenBuffer(DISP_W, DISP_H, DISP_CHUNK_H);

void OswHal::setupButtons(void) {
  pinMode(BTN_1, INPUT);
  pinMode(BTN_2, INPUT);
  pinMode(BTN_3, INPUT);
}

long waitUntil(uint8_t pin, uint8_t targetState) {
  long now = millis();
  delay(20);  // minimal debounce

  while (digitalRead(pin) != targetState) {
    delay(10);
  }

  return millis() - now;
}

void OswHal::checkButtons(void) {
  _btn1Down = false;
  _btn2Down = false;
  _btn3Down = false;

  if (millis() - _lastBtn1Down > BTN_CLICK_TIMEOUT && digitalRead(BTN_1) == LOW) {
    _btn1Down = waitUntil(BTN_1, HIGH);
    _lastBtn1Down = millis();
  }

  if (millis() - _lastBtn2Down > BTN_CLICK_TIMEOUT && digitalRead(BTN_2) == HIGH) {
    _btn2Down = waitUntil(BTN_2, LOW);
    _lastBtn2Down = millis();
  }

  if (millis() - _lastBtn3Down > BTN_CLICK_TIMEOUT && digitalRead(BTN_3) == HIGH) {
    _btn3Down = waitUntil(BTN_3, LOW);
    _lastBtn3Down = millis();
  }
}

long OswHal::btn1Down(void) { return _btn1Down; }
long OswHal::btn2Down(void) { return _btn2Down; }
long OswHal::btn3Down(void) { return _btn3Down; }