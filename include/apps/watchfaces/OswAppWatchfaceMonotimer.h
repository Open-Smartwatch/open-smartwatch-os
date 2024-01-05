#pragma once

#include <osw_hal.h>
#include <osw_ui.h>

#include <OswAppV2.h>

class OswAppWatchfaceMonotimer : public OswAppV2 {
  public:
    constexpr static const char* APP_ID = "osw.wf.mono";

    const char* getAppId() override;
    const char* getAppName() override;

    void onStart() override;
    void onLoop() override;
    void onDraw() override;
    void onStop() override;
    void onButton(Button id, bool up, ButtonStateNames state) override;

    static void drawHour();

  private:
    time_t lastTime = 0;

    void drawWatch();
    void drawNShiftedTicks(Graphics2D* gfx, uint8_t cx, uint8_t cy, uint8_t r1, uint8_t r2, uint8_t nTicks, float shift, uint16_t color);
    void drawNShiftedMaskedTicks(Graphics2D* gfx, uint8_t cx, uint8_t cy, uint8_t r1, uint8_t r2, uint8_t nTicks, float shift, uint16_t m, uint16_t color);
};
