#ifndef OSW_APP_WATCHFACE_DUAL_H
#define OSW_APP_WATCHFACE_DUAL_H

#include <osw_hal.h>
#include <osw_ui.h> 

#include "osw_app.h"

class OswAppWatchfaceDual : public OswApp {
 public:
  OswAppWatchfaceDual(void) { ui = OswUI::getInstance(); };
  virtual void setup() override;
  virtual void loop() override;
  virtual void stop() override;
  ~OswAppWatchfaceDual(){};
  //for dual-time
  void drawAnim();
  static void drawProgressBar(uint8_t cx, uint8_t cy, uint8_t jump, uint8_t length, uint8_t value,float angle, uint8_t radius, uint16_t color, int* goal=nullptr);

 private:
  OswUI* ui;
  // Blank to make the 2 clocks easier to recognize
};

#endif
