#pragma once

#include <osw_hal.h>
#include <osw_ui.h>

#include <OswAppV2.h>

class OswAppWatchfaceDual : public OswAppV2 {
  public:
    constexpr static const char* APP_ID = "osw.wf.dual";

    const char* getAppId() override;
    const char* getAppName() override;

    void onStart() override;
    void onLoop() override;
    void onDraw() override;
    void onButton(Button id, bool up, ButtonStateNames state) override;

    // for dual-time
    static void drawProgressBar(OswUI* ui,uint8_t cx, uint8_t cy, uint8_t jump, uint8_t length, uint8_t value,float angle, uint8_t radius, uint16_t color, uint8_t* goal=nullptr);
    void drawAnimSec();

  private:
    time_t lastTime = 0;
};
