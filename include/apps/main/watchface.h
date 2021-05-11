#ifndef OSW_APP_WATCHFACE_H
#define OSW_APP_WATCHFACE_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppWatchface : public OswApp {
 public:
  OswAppWatchface(void){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppWatchface(){};

 private:
    uint16_t primaryColor;
};

#endif