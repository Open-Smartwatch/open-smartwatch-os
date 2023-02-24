#ifndef OSW_APP_HELLO_WORLD_H
#define OSW_APP_HELLO_WORLD_H

#include <osw_hal.h>

#include <OswAppV2.h>

class OswAppTutorial : public OswAppV2 {
  public:
    OswAppTutorial();
    virtual ~OswAppTutorial();

    const char* getAppId() override;
    const char* getAppName() override;

    void onStart() override;
    void onLoop() override;
    void onDraw() override;
    void onStop() override;

    virtual void onButton(int id, bool up, OswAppV2::ButtonStateNames state) override;
#ifdef OSW_EMULATOR
    void onLoopDebug() override;
#endif

    bool changeRootAppIfNecessary();
  private:
    OswAppV2* previousRootApp = nullptr;
    OswIcon oswIcon;
    unsigned screen = 0;
    unsigned currentScreen = 0;
    unsigned char hsv = 0;
    bool gotButtonShort = false;
    bool gotButtonLong = false;
    bool gotButtonVeryLong = false;
    bool gotButtonDouble = false;
    Preferences nvs;
    unsigned long timeout = 0;
};

#endif
