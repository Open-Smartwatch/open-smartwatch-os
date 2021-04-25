#ifndef OSW_APP_WATER_LEVEL_H
#define OSW_APP_WATER_LEVEL_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppWaterLevel : public OswApp {
 public:
  OswAppWaterLevel(void){};
  void debug(OswHal* hal);
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppWaterLevel(){};

 private:
};

#endif