#ifndef OSW_APP_WATCHFACEBINARYA_H
#define OSW_APP_WATCHFACEBINARYA_H

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"

class OswAppWatchfaceBinary : public OswApp {
 public:
  OswAppWatchfaceBinary(void) { ui = OswUI::getInstance(); };
  virtual void setup() override;
  virtual void loop() override;
  virtual void stop() override;
  ~OswAppWatchfaceBinary(){};

 private:
  uint16_t primaryColor;
  void drawWatch(Graphics2D* gfx2d);
  OswUI* ui;
};

#endif
