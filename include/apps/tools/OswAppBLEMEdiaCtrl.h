#ifdef OSW_FEATURE_BLE_MEDIA_CTRL
#pragma once

#include <osw_hal.h>
#include "osw_app.h"

class OswAppBLEMEdiaCtrl : public OswApp {
  public:
    OswAppBLEMEdiaCtrl(void) {};
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppBLEMEdiaCtrl() {};

  private:
};
#endif