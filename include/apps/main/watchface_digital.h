#ifndef OSW_APP_WATCHFACE_DIGITAL_H
#define OSW_APP_WATCHFACE_DIGITAL_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppWatchfaceDigital : public OswApp {
 public:
  OswAppWatchfaceDigital(void){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppWatchfaceDigital(){};

 private:
  bool use24hours = true;
  bool useMMDDYYYY = false;
};

#endif