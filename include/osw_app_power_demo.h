#ifndef OSW_APP_POWER_DEMO_H
#define OSW_APP_POWER_DEMO_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppPowerDemo : public OswApp {
 public:
  OswAppPowerDemo(void){};
  void loop(OswHal* hal);
  ~OswAppPowerDemo(){};

 private:
};

#endif