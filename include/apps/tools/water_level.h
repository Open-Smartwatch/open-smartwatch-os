#ifndef OSW_APP_WATER_LEVEL_H
#define OSW_APP_WATER_LEVEL_H

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"

class OswAppWaterLevel : public OswApp {
 public:
  OswAppWaterLevel(void) { ui = OswUI::getInstance(); };
  virtual void setup() override;
  virtual void loop() override;
  virtual void stop() override;
  void debug();
  void circlesDisplay();
  void drawBar(const float value, char text, const int x);
  void barsDisplay();
  ~OswAppWaterLevel(){};

 private:
  OswUI* ui;
};

#endif
