#ifndef OSW_APP_PAULS_WATCHFACE_H
#define OSW_APP_PAULS_WATCHFACE_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppWatchface : public OswApp {
 public:
  OswAppWatchface(void){};
  void loop(OswHal* hal);
  ~OswAppWatchface(){};

 private:
};

#endif