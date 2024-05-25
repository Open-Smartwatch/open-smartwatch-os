#ifdef OSW_FEATURE_BLE_MEDIA_CTRL
#pragma once

#include <osw_hal.h>
#include <OswAppV1.h>

class OswAppBLEMediaCtrl : public OswApp {
  public:
    OswAppBLEMediaCtrl(void) {};
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppBLEMediaCtrl() {};

  private:
};
#endif
