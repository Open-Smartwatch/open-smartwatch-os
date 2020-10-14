
#include "osw_hal.h"
#include "osw_pins.h"

// Graphics2D screenBuffer(DISP_W, DISP_H, DISP_CHUNK_H);

void OswHal::setupButtons(void) {
  pinMode(BTN_1, INPUT);
  pinMode(BTN_2, INPUT);
  pinMode(BTN_3, INPUT);
}

void OswHal::checkButtons(void) {
  btn1Down = false;
  btn2Down = false;
  btn3Down = false;

  if (millis() - lastBtn1Down > BTN_CLICK_TIMEOUT && digitalRead(BTN_1) == LOW) {
    btn1Down = true;
    lastBtn1Down = millis();
  }

  if (millis() - lastBtn2Down > BTN_CLICK_TIMEOUT && digitalRead(BTN_2) == HIGH) {
    btn2Down = true;
    lastBtn2Down = millis();
  }

  if (millis() - lastBtn3Down > BTN_CLICK_TIMEOUT && digitalRead(BTN_3) == HIGH) {
    btn3Down = true;
    lastBtn3Down = millis();
  }
}