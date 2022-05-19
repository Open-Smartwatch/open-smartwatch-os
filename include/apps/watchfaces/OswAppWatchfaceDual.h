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
  void digitalClock(short timeZone, const char str[], uint8_t titleCoordX, bool showSecond, uint8_t commonCoord, uint8_t AMPMCoord);
  
  static void digitalDate(uint8_t dateCoordX, short timeZone);
  static void digitalTime(short timeZone, bool showSecond, uint8_t timeCoord, uint8_t AMPMCoord);
  static const uint8_t startCoord = 15;  // temporary

 private:
  OswUI* ui;
  // Blank to make the 2 clocks easier to recognize
};

#endif
