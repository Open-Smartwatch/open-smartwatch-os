#pragma once
#include OSW_TARGET_PLATFORM_HEADER
#ifdef OSW_FEATURE_STATS_STEPS

#include <osw_hal.h>
#include <osw_ui.h>
#include <OswAppV1.h>

class OswAppStepStats : public OswApp {
  public:
    OswAppStepStats(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
#ifdef OSW_EMULATOR
    virtual void loopDebug() override;
#endif
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
