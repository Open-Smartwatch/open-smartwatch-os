
#include "driver/rtc_io.h"
#include "osw_hal.h"
#include "osw_pins.h"

// Graphics2D screenBuffer(DISP_W, DISP_H, DISP_CHUNK_H);

void OswHal::setupButtons(void) {
  // rtc_gpio_deinit(GPIO_NUM_0);
  // rtc_gpio_deinit(GPIO_NUM_10);
  // rtc_gpio_deinit(GPIO_NUM_13);
  pinMode(BTN_1, INPUT);
  pinMode(BTN_2, INPUT);
  pinMode(BTN_3, INPUT);
}

long waitUntil(OswHal* hal, uint8_t pin, uint8_t targetState) {
  long now = millis();
  delay(20);  // minimal debounce
  bool notifiedAppSwitch = false;
  bool notifiedSleep = false;

  while (digitalRead(pin) != targetState) {
    delay(10);
    if (pin == BTN_1) {
      if (!notifiedAppSwitch && millis() - now > BTN_1_APP_SWITCH_TIMEOUT) {
        notifiedAppSwitch = true;
        hal->getCanvas()->getGraphics2D()->fillFrame(120 - 4, 230 - 4, 8, 8, rgb565(255, 255, 255));
        hal->flushCanvas();
      } else if (!notifiedSleep && millis() - now > BTN_1_SLEEP_TIMEOUT) {
        notifiedSleep = true;
        hal->getCanvas()->getGraphics2D()->fillCircle(120, 230, 9, rgb565(128, 128, 128));
        hal->getCanvas()->getGraphics2D()->fillCircle(120, 230, 8, rgb565(255, 255, 255));
        hal->getCanvas()->getGraphics2D()->fillCircle(123, 230, 6, rgb565(0, 0, 0));
        hal->flushCanvas();
      }
    }
  }

  return millis() - now;
}

void OswHal::checkButtons(void) {
  _btn1Down = false;
  _btn2Down = false;
  _btn3Down = false;

  if (millis() - _lastBtn1Down > BTN_CLICK_TIMEOUT && digitalRead(BTN_1) == LOW) {
    _btn1Down = waitUntil(this, BTN_1, HIGH);
    _lastBtn1Down = millis();
  }

  if (millis() - _lastBtn2Down > BTN_CLICK_TIMEOUT && digitalRead(BTN_2) == HIGH) {
    _btn2Down = waitUntil(this, BTN_2, LOW);
    _lastBtn2Down = millis();
  }

  if (millis() - _lastBtn3Down > BTN_CLICK_TIMEOUT && digitalRead(BTN_3) == HIGH) {
    _btn3Down = waitUntil(this, BTN_3, LOW);
    _lastBtn3Down = millis();
  }
}

long OswHal::btn1Down(void) { return _btn1Down; }
long OswHal::btn2Down(void) { return _btn2Down; }
long OswHal::btn3Down(void) { return _btn3Down; }
