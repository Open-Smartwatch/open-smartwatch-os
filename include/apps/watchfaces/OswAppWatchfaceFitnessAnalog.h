#include <osw_hal.h>
#include <osw_ui.h>
#include <OswAppV2.h>

#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1

// if you want a cool background image, enable the following define
//#define GIF_BG

#ifdef GIF_BG
class OswAppGifPlayer;
#endif
class OswAppWatchfaceFitnessAnalog : public OswAppV2 {
  public:
    constexpr static const char* APP_ID = "osw.wf.afit";

    const char* getAppId() override;
    const char* getAppName() override;

    void onStart();
    void onLoop() override;
    void onDraw() override;
    void onButton(Button id, bool up, ButtonStateNames state) override;
    void onStop() override;

    static uint32_t calculateDistance(uint32_t steps);

    ~OswAppWatchfaceFitnessAnalog() {}

  private:
    time_t lastTime = 0;
    unsigned screen = 0;

    void showFitnessTracking(OswHal* hal);
    void drawWatchFace(OswHal* hal, const OswTime& oswTime);
    void drawDateFace(OswHal* hal, const OswDate& oswDate, const OswTime& oswTime);

#ifdef GIF_BG
    OswAppGifPlayer* bgGif = nullptr;
#endif
};
#endif
