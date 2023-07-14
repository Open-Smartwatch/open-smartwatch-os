#pragma once

#include <algorithm>
#include <OswAppV2.h>

#include "../../../include/apps/OswAppDrawer.h"
#include "../../../include/apps/clock/OswAppTimer.h"
#include "../../../include/apps/clock/OswAppAlarm.h"
#include "TestAppV2Compat.h"
#include "globals.h"
using OswGlobals::main_mainDrawer;

class TestDrawer
{
public:
    static OswAppTimer *getTimer()
    {
        // Iterate to the second element (timer) of the TOOLS apps list
        auto OswAppTimerIt = std::next((main_mainDrawer.apps.at(LANG_TOOLS).begin())); 
        TestDrawer::lazyInitTimer = &(*OswAppTimerIt);

        // Cast OswAppV2 to OswAppV2Compat, because we need OswAppV1 app from OswAppV2Compat
        OswAppV2Compat &oswAppV2CompatTimer = static_cast<OswAppV2Compat&>(*lazyInitTimer->get());

        // Get OswAppV1 from OswAppV2Compat
        OswAppV1 *OswAppV1Timer = TestAppV2Compat::getAppV1(oswAppV2CompatTimer);

        // Cast to timer class
        return reinterpret_cast<OswAppTimer *>(OswAppV1Timer);
    }
    static void switchToTimer()
    {
        main_mainDrawer.current = lazyInitTimer;
    }
    static OswAppAlarm *getAlarm()
    {
        // Iterate to the third element (alarm) of the TOOLS apps list
        auto OswAppAlarmIt = std::next(std::next((main_mainDrawer.apps.at(LANG_TOOLS).begin())));
        TestDrawer::lazyInitAlarm = &(*OswAppAlarmIt);

        // Cast OswAppV2 to OswAppV2Compat, because we need OswAppV1 app from OswAppV2Compat
        OswAppV2Compat &oswAppV2CompatAlarm = static_cast<OswAppV2Compat&>(*lazyInitAlarm->get());

        // Get OswAppV1 from OswAppV2Compat
        OswAppV1 *OswAppV1Alarm = TestAppV2Compat::getAppV1(oswAppV2CompatAlarm);
        
        // Cast to alarm class
        return reinterpret_cast<OswAppAlarm *>(OswAppV1Alarm);
    }
    static void switchToAlarm()
    {
        main_mainDrawer.current = lazyInitAlarm;
    }
private:
    inline static OswAppDrawer::LazyInit* lazyInitTimer;
    inline static OswAppDrawer::LazyInit* lazyInitAlarm;
};