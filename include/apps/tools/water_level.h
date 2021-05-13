#ifndef OSW_APP_WATER_LEVEL_H
#define OSW_APP_WATER_LEVEL_H

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"

class OswAppWaterLevel : public OswApp {
 public:
  OswAppWaterLevel(void) { ui = OswUI::getInstance(); };
  void debug(OswHal* hal);
  void setup(OswHal* hal);
  void circlesDisplay(OswHal* hal);
  void drawBar(OswHal* hal, const float value, char text, const int x);
  void barsDisplay(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppWaterLevel(){};

 private:
  OswUI* ui;
};

#endif
