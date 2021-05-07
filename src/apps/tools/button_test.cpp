
#include "./apps/tools/button_test.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

void OswButtonTest::setup(OswHal* hal) {

}

void OswButtonTest::loop(OswHal* hal) {

  for(uint8_t i = BUTTON_1; i <= BUTTON_3 ; i++){
    if(hal->btnHasGoneDown(i)){
          lastValue = goneDown;
          lastButton = i;
    }
    if(hal->btnHasGoneUp(i)){
        lastValue = goneUp;
        lastButton = i;
    }
    if(hal->btnIsLongPress(i)){
        lastValue = longPress;
        lastButton = i;
    }
  }
  
  hal->gfx()->fill(0);
  hal->gfx()->setTextSize(4);
  hal->gfx()->setTextCursor(40, 120);
  hal->gfx()->print(lastValue);
  if(lastButton < 255){
    hal->gfx()->setTextCursor(40, 60);
    hal->gfx()->print(lastButton);
  }
  hal->requestFlush();
}

void OswButtonTest::stop(OswHal* hal) {

}
