#ifdef OSW_FEATURE_STATS_STEPS

#pragma once

#include <osw_hal.h>
#include <osw_ui.h>
#include "osw_app.h"

class OswAppDistStats : public OswApp {
  public:
    OswAppDistStats(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
#ifdef OSW_EMULATOR
    virtual void loopDebug() override;
#endif
    virtual void stop() override;
    ~OswAppDistStats() {};

  private:
    void showStickChart();
    void drawChart();
    OswUI* ui;
    int32_t cursorPos = 0; // WeekDay position
};

#endif