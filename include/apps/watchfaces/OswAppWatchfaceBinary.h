#pragma once

#include <osw_hal.h>
#include <osw_ui.h>

#include <OswAppV2.h>

class OswAppWatchfaceBinary : public OswAppV2 {
  public:
    constexpr static const char* APP_ID = "osw.wf.binary";

    const char* getAppId() override;
    const char* getAppName() override;

    void onStart() override;
    void onLoop() override;
    void onDraw() override;
    void onButton(Button id, bool up, ButtonStateNames state) override;

  private:
    time_t lastTime = 0;
    uint16_t primaryColor;

    void drawWatch();
};
