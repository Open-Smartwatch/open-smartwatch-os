
#include "./apps/watchfaces/watchface_binary.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>

#include "bma400_defs.h"

#define COLOR_SECxOND rgb565(231, 111, 81)
#define COLOR_MIxNUTE rgb565(244, 162, 97)
#define COLOR_HOxUR rgb565(42, 157, 143)
#define COLOR_BLAxCK rgb565(0, 0, 0)
#define COLOR_WHxITE rgb565(255, 255, 255)

void OswAppWatchfaceBinary::drawWatch(Graphics2D* gfx2d) {
  uint32_t second = 0;
  uint32_t minute = 0;
  uint32_t hour = 0;
  bool afterNoon = false;
  OswHal* hal = OswHal::getInstance();
  hal->getLocalTime(&hour, &minute, &second, &afterNoon);

  uint16_t width = hal->gfx()->getWidth();
  uint16_t height = hal->gfx()->getHeight();

  hal->gfx()->fill(ui->getBackgroundColor());

  //hours
  for(uint8_t i = 0; i < 5 ; i++ ){
    uint32_t b = pow(2, i);
    if((hour & b) == 0){
      hal->gfx()->drawFrame(width - (((width - 32) / 8) * i + 64) - 32, height / 2 - 16, 8, 8, ui->getWarningColor());
    }else{
      hal->gfx()->fillFrame(width - (((width - 32) / 8) * i + 64) - 32, height / 2 - 16, 8, 8, ui->getWarningColor());
    }
  }
  //minutes
  for(uint8_t i = 0; i < 6 ; i++ ){
    uint32_t b = pow(2, i);
    if((minute & b) == 0){
      hal->gfx()->drawFrame(width - (((width - 32) / 8) * i + 64) - 32, height / 2, 8, 8, ui->getDangerColor());
    }else{
      hal->gfx()->fillFrame(width - (((width - 32) / 8) * i + 64) - 32, height / 2, 8, 8, ui->getDangerColor());
    }
  }
  //seconds
  for(uint8_t i = 0; i < 6 ; i++ ){
    uint32_t b = pow(2, i);
    if((second & b) == 0){
      hal->gfx()->drawFrame(width - (((width - 32) / 8) * i + 64) - 32, height / 2 + 16, 8, 8, ui->getInfoColor());
    }else{
      hal->gfx()->fillFrame(width - (((width - 32) / 8) * i + 64) - 32, height / 2 + 16, 8, 8, ui->getInfoColor());
    }
  }

  //cosmetic
  hal->gfx()->drawLine(width /2 + 40, height / 8 * 1, width /2 + 40, height / 8 * 7, ui->getForegroundColor());

  //steps
  uint32_t steps = hal->environment->getStepsToday();
  hal->gfx()->setTextSize(1);
  ui->resetTextColors();
  hal->gfx()->setTextCursor(width /2 + 48, height / 2 + 32 + 16);
  hal->gfx()->print(LANG_WATCHFACE_BINARY_STEPS);

  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextCursor(width /2 + 48, height / 2 + 32);
  hal->gfx()->print("0x");
  hal->gfx()->print(steps, HEX);
}

void OswAppWatchfaceBinary::setup() {

}

void OswAppWatchfaceBinary::loop() {
  OswHal* hal = OswHal::getInstance();
  if (hal->btnHasGoneDown(BUTTON_3)) {
    hal->increaseBrightness(25);
  }
  if(hal->btnHasGoneDown(BUTTON_2)) {
    hal->decreaseBrightness(25);
  }
  drawWatch(hal->getCanvas()->getGraphics2D());
  hal->requestFlush();
}

void OswAppWatchfaceBinary::stop() {}
