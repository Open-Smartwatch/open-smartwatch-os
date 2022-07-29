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
    static void drawInfoPanel(OswUI* ui, uint32_t pos, uint32_t lastWeekData, uint32_t todayData, uint32_t average, uint32_t total, const String& unit = String(""));

  private:
    void showStickChart();
    void drawChart();
    int32_t cursorPos=0;
    OswUI* ui;
};

#endif