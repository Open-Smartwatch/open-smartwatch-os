#pragma once
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_ui.h>

#include <vector>

enum OswAppSwitcherType { SHORT_PRESS, LONG_PRESS };

class OswAppSwitcher : public OswApp {
  public:
    OswAppSwitcher(Button btn, OswAppSwitcherType type, bool enableAutoSleep, bool enableSleep, uint16_t* rtcAppIndex) {
        _btn = btn;
        _type = type;
        _enableAutoSleep = enableAutoSleep;
        _enableSleep = enableSleep;
        _rtcAppIndex = rtcAppIndex;
    }
    OswAppSwitcher() {};
    virtual void setup() override;
    virtual void loop() override;
#ifdef OSW_EMULATOR
    virtual void loopDebug() override;
#endif
    virtual void stop() override;
    void paginationDisable();
    void paginationEnable();
    void registerApp(OswApp* app);
    ~OswAppSwitcher() {};

  private:
    void cycleApp();
    void sleep();
    Button _btn = BUTTON_1;
    OswAppSwitcherType _type = LONG_PRESS;
    std::vector<OswApp*> _apps;
    uint16_t* _rtcAppIndex;
    uint16_t _appCount = 0;
    bool _pagination = true;
    bool _paginationIndicator = false;
    bool _enableAutoSleep = false;
    bool _checked = false;
    bool _enableSleep;
    bool _doSleep = false;
    bool _doSwitch = false;
    long appOnScreenSince = 0;
    unsigned long _timeForLongPress = APPSWITCHER_LONG_PRESS;
    unsigned long _timeForSleepPress = APPSWITCHER_SLEEP_TIMEOUT;
};
