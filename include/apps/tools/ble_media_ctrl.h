#ifndef OSW_APP_BLE_MEDIA_CTRL_H
#define OSW_APP_BLE_MEDIA_CTRL_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppBLEMEdiaCtrl : public OswApp {
 public:
  OswAppBLEMEdiaCtrl(void){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppBLEMEdiaCtrl(){};

 private:
};

#endif