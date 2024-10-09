#pragma once
#include OSW_TARGET_PLATFORM_HEADER
#ifdef OSW_FEATURE_STATS_STEPS

#include <osw_hal.h>
#include <osw_ui.h>
#include <OswAppV1.h>

class OswAppKcalStats : public OswApp {
  public:
    OswAppKcalStats(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
#ifdef OSW_EMULATOR
    virtual void loopDebug() override;
#endif
    virtual void stop() override;
    ~OswAppKcalStats() {};

  private:
    uint8_t cursorPos = 6;  // WeekDay position
    void drawCurvedChart();
    void showCurvedChart();
    OswUI* ui;
};

#endif
