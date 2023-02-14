#pragma once

#include "../../../include/apps/main/switcher.h"
#include "../../../include/apps/clock/OswAppTimer.h"
#include "../../../include/apps/clock/OswAppAlarm.h"

extern OswAppSwitcher clockAppSwitcher; // Used to get OswAppTimer instance

class TestSwitcher
{
public:
    static OswAppTimer* getTimer() { return static_cast<OswAppTimer*>(clockAppSwitcher._apps[1]); }
    static OswAppAlarm* getAlarm() { return static_cast<OswAppAlarm*>(clockAppSwitcher._apps[2]); }
};