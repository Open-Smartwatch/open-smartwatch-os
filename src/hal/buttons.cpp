
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

  long now = millis();
  _btn1DownSince = 0;
  _btn2DownSince = 0;
  _btn3DownSince = 0;
  _btn1UpSince = now;  // we need the +1 for the detection to begin working
  _btn2UpSince = now;
  _btn3UpSince = now;
}

void OswHal::checkButtons(void) {
  if (digitalRead(BTN_1) == LOW && millis() - _btn1UpSince > BTN_CLICK_TIMEOUT) {
    if (_btn1DownSince < _btn1UpSince) {
      _btn1DownSince = millis();
    }
  } else if (_btn1UpSince < _btn1DownSince) {
    _btn1UpSince = millis();
  }

  if (digitalRead(BTN_2) == HIGH && millis() - _btn2UpSince > BTN_CLICK_TIMEOUT) {
    if (_btn2DownSince < _btn2UpSince) {
      _btn2DownSince = millis();
    }
  } else if (_btn2UpSince < _btn2DownSince) {
    _btn2UpSince = millis();
  }

  if (digitalRead(BTN_3) == HIGH && millis() - _btn3UpSince > BTN_CLICK_TIMEOUT) {
    if (_btn3DownSince < _btn3UpSince) {
      _btn3DownSince = millis();
    }
  } else if (_btn3UpSince < _btn3DownSince) {
    _btn3UpSince = millis();
  }
}

long OswHal::btn1Down(void) { return _btn1UpSince < _btn1DownSince ? millis() - _btn1DownSince : 0; }
long OswHal::btn2Down(void) { return _btn2UpSince < _btn2DownSince ? millis() - _btn2DownSince : 0; }
long OswHal::btn3Down(void) { return _btn3UpSince < _btn3DownSince ? millis() - _btn3DownSince : 0; }

void OswHal::clearBtn1(void) { _btn1UpSince = millis(); }
void OswHal::clearBtn2(void) { _btn2UpSince = millis(); }
void OswHal::clearBtn3(void) { _btn3UpSince = millis(); }
