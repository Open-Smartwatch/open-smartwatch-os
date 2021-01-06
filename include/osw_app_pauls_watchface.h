#ifndef OSW_APP_PAULS_WATCHFACE_H
#define OSW_APP_PAULS_WATCHFACE_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppPaulsWatchface : public OswApp {
 public:
  OswAppPaulsWatchface(void){};
  void loop(OswHal* hal);
  ~OswAppPaulsWatchface(){};

 private:
};

#endif