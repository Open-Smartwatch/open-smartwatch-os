#ifndef OSW_APP_BLE_MEDIA_CTRL_H
#define OSW_APP_BLE_MEDIA_CTRL_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppBLEMEdiaCtrl : public OswApp {
 public:
  OswAppBLEMEdiaCtrl(void){};
  virtual void setup() override;
  virtual void loop() override;
  virtual void stop() override;
  ~OswAppBLEMEdiaCtrl(){};

 private:
};

#endif