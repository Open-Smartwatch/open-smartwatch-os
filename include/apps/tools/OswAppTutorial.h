#ifndef OSW_APP_HELLO_WORLD_H
#define OSW_APP_HELLO_WORLD_H

#include <osw_hal.h>

#include <OswAppV2.h>

class OswAppTutorial : public OswAppV2 {
  public:
    OswAppTutorial();

    const char* getAppId() override;
    const char* getAppName() override;

    void onStart() override;
    void onLoop() override;
    void onDraw() override;
    void onStop() override;

    virtual void onButton(int id, bool down, bool shortPress, bool longPress);
#ifdef OSW_EMULATOR
    void onLoopDebug() override;
#endif
  private:
    OswIcon oswIcon;
    unsigned screen;
    unsigned char hsv;
};

#endif
