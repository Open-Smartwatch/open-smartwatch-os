#pragma once
#include OSW_TARGET_PLATFORM_HEADER
#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1

#include <osw_hal.h>
#include <osw_ui.h>

#include <OswAppV2.h>

class OswAppWatchfaceFitness : public OswAppV2 {
  public:
    constexpr static const char* APP_ID = "osw.wf.fit";

    const char* getAppId() override;
    const char* getAppName() override;

    void onStart() override;
    void onLoop() override;
    void onDraw() override;
    void onButton(Button id, bool up, ButtonStateNames state) override;

    static float calculateDistance(uint32_t steps);
    static uint32_t calculateKcalorie(uint32_t steps);

  private:
    time_t lastTime = 0;

    void showFitnessTracking();
};
#endif