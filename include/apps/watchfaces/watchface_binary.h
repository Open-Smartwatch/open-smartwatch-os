#ifndef OSW_APP_WATCHFACEBINARYA_H
#define OSW_APP_WATCHFACEBINARYA_H

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"

class OswAppWatchfaceBinary : public OswApp {
 public:
  OswAppWatchfaceBinary(void) { ui = OswUI::getInstance(); };
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppWatchfaceBinary(){};

 private:
  uint16_t primaryColor;
  void drawWatch(OswHal* hal, Graphics2D* gfx2d);
  OswUI* ui;
};

#endif
