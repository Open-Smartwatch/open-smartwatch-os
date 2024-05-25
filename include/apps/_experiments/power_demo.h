#ifndef OSW_APP_POWER_DEMO_H
#define OSW_APP_POWER_DEMO_H

#include <osw_hal.h>

#include <OswAppV1.h>

class OswAppPowerDemo : public OswApp {
  public:
    OswAppPowerDemo(void) {};
    virtual void loop() override;
    ~OswAppPowerDemo() {};

  private:
};

#endif
