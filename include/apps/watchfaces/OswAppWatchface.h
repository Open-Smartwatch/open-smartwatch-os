#pragma once

#include <osw_hal.h>
#include <osw_ui.h>

#include <OswAppV2.h>
#ifdef ANIMATION
#include <animations/anim_matrix.h>
#endif

class OswAppGifPlayer;
class OswAppWatchface : public OswAppV2 {
  public:
    constexpr static const char* APP_ID = "osw.wf.analog";
    const char* getAppId() override;
    const char* getAppName() override;

    void onStart() override;
    void onLoop() override;
    void onDraw() override;
    void onStop() override;
    void onButton(int id, bool up, ButtonStateNames state) override;

#ifdef OSW_FEATURE_STATS_STEPS
    static void drawStepHistory(OswUI* ui, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t max);
#endif
    static void handleButtonDefaults();
    static void onButtonDefaults(OswAppV2& app, int id, bool up, ButtonStateNames state);
  private:
    time_t lastTime = 0;

    void drawWatch();
#ifdef ANIMATION
    AnimMatrix* matrix;
#endif
#ifdef GIF_BG
    OswAppGifPlayer* bgGif = nullptr;
#endif
};
