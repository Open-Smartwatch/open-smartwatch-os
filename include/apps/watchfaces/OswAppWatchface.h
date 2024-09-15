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
    void onButton(Button id, bool up, ButtonStateNames state) override;

#ifdef OSW_FEATURE_STATS_STEPS
    static void drawStepHistory(OswUI* ui, uint8_t posX, uint8_t posY, uint8_t width, uint8_t height, uint32_t max);
#endif
    static void addButtonDefaults(std::array<ButtonStateNames, BTN_NUMBER>& knownButtonStates);
    static bool onButtonDefaults(OswAppV2& app, Button id, bool up, ButtonStateNames state);
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
