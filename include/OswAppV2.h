#pragma once
#include <array>

#include <OswIcon.h>
#include <osw_hal.h>

class OswHal;
class OswAppV2 {
  public:
    enum ViewFlags: char {
      NONE = 0,
      NO_OVERLAYS = 1
    };
    enum ButtonStateNames: char {
      UNDEFINED = 0,
      SHORT_PRESS = 1,
      LONG_PRESS = 2,
      VERY_LONG_PRESS = 4,
      DOUBLE_PRESS = 8
    };
    bool needsRedraw = false;

    OswAppV2();

    virtual const char* getAppId() = 0;
    virtual const char* getAppName() = 0;
    virtual OswIcon& getAppIcon();

    virtual void onStart();
    virtual void onLoop();
    virtual void onDraw();
    virtual void onStop();

    virtual void onButton(int id, bool up, ButtonStateNames state);
#ifdef OSW_EMULATOR
    virtual void onLoopDebug(); // By default no debug loop (GUI) is implemented
#endif

    ViewFlags getViewFlags();
  protected:
    OswHal* hal = nullptr; // You guys are needing that anyways (but you often cache incorrectly), so it is now given to you <3
    std::array<ButtonStateNames, NUM_BUTTONS> knownButtonStates; // Bitmask of known button states (ignores the DOUBLE_PRESS state by default), use this to ignore unhandled button states
    ViewFlags viewFlags = ViewFlags::NONE;
    OswIcon& getDefaultAppIcon();

  private:
    static OswIcon defaultAppIcon;
    std::array<unsigned long, NUM_BUTTONS> buttonDownSince;
    std::array<ButtonStateNames, NUM_BUTTONS> buttonLastSentState;
    std::array<unsigned long, NUM_BUTTONS> buttonDoubleShortTimeout;

    void updateCachedHal();
};