#pragma once
#include <array>

#include <OswIcon.h>
#include <icon/OswIconProgmem.h> // used for own default app icon
#include <osw_hal.h>
#include <osw_ui.h>

class OswHal;
class OswAppV2 {
  public:
    enum ViewFlags: char {
        NONE = 0,
        NO_OVERLAYS = 1,
        KEEP_DISPLAY_ON = 2,
        NO_FPS_LIMIT = 4
    };
    enum ButtonStateNames: char {
        UNDEFINED = 0,
        SHORT_PRESS = 1,
        LONG_PRESS = 2,
        VERY_LONG_PRESS = 4,
        DOUBLE_PRESS = 8
    };

    OswAppV2();
    virtual ~OswAppV2() = default;

    virtual const char* getAppId() = 0;
    virtual const char* getAppName() = 0;
    virtual const OswIcon& getAppIcon();

    virtual void onStart();
    virtual void onLoop();
    virtual void onDraw();
    virtual void onDrawOverlay();
    virtual void onStop();

    virtual void onButton(Button id, bool up, ButtonStateNames state);
#ifdef OSW_EMULATOR
    virtual void onLoopDebug(); // By default no debug loop (GUI) is implemented
#endif

    virtual const ViewFlags& getViewFlags();
    virtual bool getNeedsRedraw();
    virtual void resetNeedsRedraw();
  protected:
    class OswHalProxy {
      public:
        // This will proxy the "->" operator to use the current instance of OswHal
        OswHal* operator->() {
            return OswHal::getInstance();
        };
        operator OswHal* () {
            return OswHal::getInstance();
        };
        // We intentionally do not provide an operation to implicitly convert to OswHal* to prevent accidental use of the wrong instance
    };
    OswHalProxy hal; // You guys are needing that anyway (but you often cache incorrectly), so it is now given to you <3
    class OswUiProxy {
      public:
        OswUI* operator->() {
            return OswUI::getInstance();
        };
        operator OswUI* () {
            return OswUI::getInstance();
        };
    };
    OswUiProxy ui;
    std::array<ButtonStateNames, BTN_NUMBER> knownButtonStates; // Bitmask of known button states, use this to ignore unhandled button states
    ViewFlags viewFlags = ViewFlags::NONE;
    bool needsRedraw = false;
    const OswIcon& getDefaultAppIcon();
    void clearKnownButtonStates();

  private:
    static OswIconProgmem defaultAppIcon;
    std::array<unsigned long, BTN_NUMBER> buttonDownSince = {0};
    std::array<ButtonStateNames, BTN_NUMBER> buttonLastSentState = {ButtonStateNames::UNDEFINED};
    std::array<unsigned long, BTN_NUMBER> buttonDoubleShortTimeout = {0};
    std::array<float, BTN_NUMBER> buttonIndicatorProgress = {0};
};