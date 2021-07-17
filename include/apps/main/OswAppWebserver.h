#ifndef OSW_APP_CONFIG_MGMT_H
#define OSW_APP_CONFIG_MGMT_H

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"

class OswAppWebserver : public OswApp {
 public:
  OswAppWebserver(void) { ui = OswUI::getInstance(); };
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppWebserver(){};

 private:
  OswUI* ui;
};

#endif
