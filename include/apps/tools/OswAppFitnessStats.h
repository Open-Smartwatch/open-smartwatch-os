#pragma once

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
