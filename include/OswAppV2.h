#pragma once
#include <OswIcon.h>
#include <osw_hal.h>

class OswHal;
class OswAppV2 {
  public:
    enum ViewFlags {
        NONE = 0,
        NO_OVERLAYS = 1
    };
    bool needsRedraw = false;

    virtual const char* getAppId() = 0;
    virtual const char* getAppName() = 0;
    virtual OswIcon& getAppIcon();

    virtual void onStart();
    virtual void onLoop();
    virtual void onDraw();
    virtual void onStop();

    virtual void onButton(int id, bool down, bool shortPress, bool longPress);
#ifdef OSW_EMULATOR
    virtual void onLoopDebug(); // By default no debug loop (GUI) is implemented
#endif

    ViewFlags getViewFlags();
  protected:
    OswHal* hal = nullptr; // You guys are needing that anyways (but you often cache incorrectly), so it is now given to you <3
    ViewFlags viewFlags = NONE;
    OswIcon& getDefaultAppIcon();

  private:
    static OswIcon defaultAppIcon;

    void updateCachedHal();
};