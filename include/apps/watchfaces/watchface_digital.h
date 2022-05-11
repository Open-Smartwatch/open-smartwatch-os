#ifndef OSW_APP_WATCHFACE_DIGITAL_H
#define OSW_APP_WATCHFACE_DIGITAL_H

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"

class OswAppWatchfaceDigital : public OswApp {
 public:
  OswAppWatchfaceDigital(void){ ui = OswUI::getInstance(); };
  static uint8_t showDateFormat;  // 0 : mm/dd/yyyy 1 : dd.mm.yyyy 2 : yy.mm/dd

  virtual void setup() override;
  virtual void loop() override;
  virtual void stop() override;
  ~OswAppWatchfaceDigital(){};

 private:
  OswUI* ui;
};

#endif
