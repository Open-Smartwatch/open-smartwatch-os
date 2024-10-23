#pragma once
#include OSW_TARGET_PLATFORM_HEADER
#ifdef OSW_FEATURE_STATS_STEPS

#include <osw_hal.h>
#include <osw_ui.h>
#include <OswAppV1.h>

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
