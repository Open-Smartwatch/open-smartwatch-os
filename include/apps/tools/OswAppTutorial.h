#pragma once

#include <optional>

#include <OswAppV2.h>

class OswAppTutorial : public OswAppV2 {
  public:
    OswAppTutorial();
    virtual ~OswAppTutorial();

    const char* getAppId() override;
    const char* getAppName() override;
    OswIcon& getAppIcon() override;

    void onStart() override;
    void onLoop() override;
    void onDraw() override;
    void onStop() override;

    virtual void onButton(Button id, bool up, OswAppV2::ButtonStateNames state) override;
#ifdef OSW_EMULATOR
    void onLoopDebug() override;
#endif

    bool changeRootAppIfNecessary();
  private:
    OswAppV2* previousRootApp = nullptr;
    static OswIconProgmem oswIcon;
    unsigned screen = 0;
    unsigned currentScreen = 0;
    unsigned char hsv = 0;
    std::optional<Button> gotButtonShort = std::nullopt;
    std::optional<Button> gotButtonLong = std::nullopt;
    std::optional<Button> gotButtonVeryLong = std::nullopt;
    std::optional<Button> gotButtonDouble = std::nullopt;
    Preferences nvs;
    time_t timeout = 0;
    time_t nextScreenOn = 0;
};
