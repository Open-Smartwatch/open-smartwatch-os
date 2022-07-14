#ifdef OSW_FEATURE_STATS_STEPS

#pragma once

#include <osw_hal.h>
#include <osw_ui.h>
#include "osw_app.h"

class OswAppStepStats : public OswApp {
  public:
    OswAppStepStats(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppStepStats() {};

  private:
    void showStickChart();
    void drawChart();
    uint32_t cursorPos;
    OswUI *ui;
};

#endif