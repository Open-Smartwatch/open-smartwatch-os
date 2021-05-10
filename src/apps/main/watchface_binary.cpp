
#include "./apps/main/watchface_binary.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>

#include "bma400_defs.h"

#define COLOR_SECOND rgb565(231, 111, 81)
#define COLOR_MINUTE rgb565(244, 162, 97)
#define COLOR_HOUR rgb565(42, 157, 143)
#define COLOR_BLACK rgb565(0, 0, 0)
#define COLOR_WHITE rgb565(255, 255, 255)

void OswAppWatchfaceBinary::drawWatch(OswHal* hal, Graphics2D* gfx2d) {
  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;
  bool afterNoon = false;
  hal->getLocalTime(&hour, &minute, &second, &afterNoon);
  
  uint16_t width = hal->gfx()->getWidth();
  uint16_t height = hal->gfx()->getHeight();


  hal->gfx()->fill(COLOR_BLACK);

  //hours
  for(uint8_t i = 0; i < 5 ; i++ ){
    uint32_t b = pow(2, 8 - i);
    if((hour & b) == 0){
      hal->gfx()->drawFrame(width - (((width - 32) / 8) * i + 64) - 32, height / 2 - 16, 8, 8, COLOR_HOUR);  
    }else{
      hal->gfx()->fillFrame(width - (((width - 32) / 8) * i + 64) - 32, height / 2 - 16, 8, 8, COLOR_HOUR);  
    }
  }
  //minutes
  for(uint8_t i = 0; i < 6 ; i++ ){
    uint32_t b = pow(2, i);
    if((minute & b) == 0){
      hal->gfx()->drawFrame(width - (((width - 32) / 8) * i + 64) - 32, height / 2, 8, 8, COLOR_MINUTE);  
    }else{
      hal->gfx()->fillFrame(width - (((width - 32) / 8) * i + 64) - 32, height / 2, 8, 8, COLOR_MINUTE);  
    }
  }
  //seconds
  for(uint8_t i = 0; i < 6 ; i++ ){
    uint32_t b = pow(2, i);
    if((second & b) == 0){
      hal->gfx()->drawFrame(width - (((width - 32) / 8) * i + 64) - 32, height / 2 + 16, 8, 8, COLOR_SECOND);  
    }else{
      hal->gfx()->fillFrame(width - (((width - 32) / 8) * i + 64) - 32, height / 2 + 16, 8, 8, COLOR_SECOND);  
    }
  }

  //cosmetic
  hal->gfx()->drawLine(width /2 + 40, height / 8 * 1, width /2 + 40, height / 8 * 7, COLOR_WHITE);
  
  //steps
  uint32_t steps = hal->getStepCount();
  hal->gfx()->setTextSize(1);
  hal->gfx()->setTextColor(COLOR_WHITE,COLOR_BLACK);
  hal->gfx()->setTextCursor(width /2 + 48, height / 2 + 32 + 16);
  hal->gfx()->print("steps");

  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextCursor(width /2 + 48, height / 2 + 32);
  hal->gfx()->print("0x");
  hal->gfx()->print(steps, HEX);
}

void OswAppWatchfaceBinary::setup(OswHal* hal) {

}

void OswAppWatchfaceBinary::loop(OswHal* hal) {

  if (hal->btnHasGoneDown(BUTTON_3)) {
    hal->increaseBrightness(25);
  }
  if(hal->btnHasGoneDown(BUTTON_2)) {
    hal->decreaseBrightness(25);
  }
  drawWatch(hal, hal->getCanvas()->getGraphics2D());
  hal->requestFlush();
}

void OswAppWatchfaceBinary::stop(OswHal* hal) {

}
