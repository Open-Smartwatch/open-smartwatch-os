#pragma once
#include OSW_TARGET_PLATFORM_HEADER
#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1

#include <osw_hal.h>
#include <osw_ui.h>
#include <OswAppV1.h>

class OswAppFitnessStats : public OswApp {
  public:
    OswAppFitnessStats(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppFitnessStats() {};
    void showFitnessTracking();

  private:
    OswUI* ui;
};
#endif
