
#include "osw_hal.h"
#include "osw_pins.h"

// Graphics2D screenBuffer(DISP_W, DISP_H, DISP_CHUNK_H);

void OswHal::setupButtons(void) {
  pinMode(BTN_1, INPUT);
  pinMode(BTN_2, INPUT);
  pinMode(BTN_3, INPUT);
}

void OswHal::checkButtons(void) {
  _btn1Down = false;
  _btn2Down = false;
  _btn3Down = false;

  if (millis() - _lastBtn1Down > BTN_CLICK_TIMEOUT && digitalRead(BTN_1) == LOW) {
    _btn1Down = true;
    _lastBtn1Down = millis();
  }

  if (millis() - _lastBtn2Down > BTN_CLICK_TIMEOUT && digitalRead(BTN_2) == HIGH) {
    _btn2Down = true;
    _lastBtn2Down = millis();
  }

  if (millis() - _lastBtn3Down > BTN_CLICK_TIMEOUT && digitalRead(BTN_3) == HIGH) {
    _btn3Down = true;
    _lastBtn3Down = millis();
  }
}

bool OswHal::btn1Down(void) { return _btn1Down; }
bool OswHal::btn2Down(void) { return _btn2Down; }
bool OswHal::btn3Down(void) { return _btn3Down; }