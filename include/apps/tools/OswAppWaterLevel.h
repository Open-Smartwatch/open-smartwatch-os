#pragma once
#include OSW_TARGET_PLATFORM_HEADER
#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1

#include <osw_hal.h>
#include <osw_ui.h>
#include <OswAppV1.h>

class OswAppWaterLevel : public OswApp {
  public:
    OswAppWaterLevel(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    void debug();
    void circlesDisplay();
    void drawBar(const float value, char text, const int x);
    void barsDisplay();
    ~OswAppWaterLevel() {};

  private:
    OswUI* ui;
    int displayMode = 1;
};
#endif
