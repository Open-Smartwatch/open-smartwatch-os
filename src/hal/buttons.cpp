
#include "driver/rtc_io.h"
#include "osw_hal.h"
#include "osw_pins.h"


#define DEFAULTLAUNCHER_LONG_PRESS 1000

// Graphics2D screenBuffer(DISP_W, DISP_H, DISP_CHUNK_H);

void OswHal::setupButtons(void) {
  // rtc_gpio_deinit(GPIO_NUM_0);
  // rtc_gpio_deinit(GPIO_NUM_10);
  // rtc_gpio_deinit(GPIO_NUM_13);
  pinMode(BTN_1, INPUT);
  pinMode(BTN_2, INPUT);
  pinMode(BTN_3, INPUT);



  //Buttons 
  long now = millis();
  _btn1DownSince = 0;
  _btn2DownSince = 0;
  _btn3DownSince = 0;
  _btn1UpSince = now;  // we need the +1 for the detection to begin working
  _btn2UpSince = now;
  _btn3UpSince = now;
  
  //Buttons (Engine)
  for(uint8_t i = 0  ; i < 3 ; i++){
    btnLastState[i] = false;
    btnIsDown[i] = false;
    btnGoneUp[i] = false;
  }

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

  //Buttons (Engine)
  btnIsDown[0] = digitalRead(BTN_1) == LOW;
  btnIsDown[1] = !digitalRead(BTN_2) == LOW;
  btnIsDown[2] = !digitalRead(BTN_3) == LOW;

  for(uint8_t i = 0  ; i < 3 ; i++){
    btnGoneUp[i] = btnLastState[i] == true && btnIsDown[i] == false;
    btnGoneDown[i] = btnLastState[i] == false && btnIsDown[i] == true;
    
    if(btnGoneDown[i]){
      btnIsDownSince[i] = millis();  
    }
    btnLongPress[i] = btnIsDownSince[i] < millis() - DEFAULTLAUNCHER_LONG_PRESS && btnIsDown[i] == true;
    btnLastState[i] = btnIsDown[i];

    if(btnSuppressUntilUpAgain[i] == true && btnIsDown[i]){
      btnGoneUp[i] = false;
      btnGoneDown[i] = false;
      btnIsDownSince[i] = millis();  
      btnLongPress[i] = false;
      btnIsDown[i] = false;
    }else{
      btnSuppressUntilUpAgain[i] = false;
    }
  }

}




long OswHal::btn1Down(void) { return _btn1UpSince < _btn1DownSince ? millis() - _btn1DownSince : 0; }
long OswHal::btn2Down(void) { return _btn2UpSince < _btn2DownSince ? millis() - _btn2DownSince : 0; }
long OswHal::btn3Down(void) { return _btn3UpSince < _btn3DownSince ? millis() - _btn3DownSince : 0; }

void OswHal::clearBtn1(void) { _btn1UpSince = millis(); }
void OswHal::clearBtn2(void) { _btn2UpSince = millis(); }
void OswHal::clearBtn3(void) { _btn3UpSince = millis(); }

//Buttons (Engine)
//btn1 = 0, btn2 = 1, btn3 = 0
bool OswHal::btnHasGoneDown(uint8_t btn){return btnGoneDown[btn];}
bool OswHal::btnHasGoneUp(uint8_t btn){return btnGoneUp[btn];}
bool OswHal::btnIsPhysicalDown(uint8_t btn){return btnIsDown[btn];}
bool OswHal::btnIsLongPress(uint8_t btn){return btnLongPress[btn];}




