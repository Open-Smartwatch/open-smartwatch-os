#pragma once

#include "../../../include/apps/clock/OswAppAlarm.h"

// This is a friend class of OswAppAlarm. It is needed to access and test private members of OswAppAlarm
class TestAlarm
{
public:
    static OswAppAlarm::AlarmState getState(OswAppAlarm &alarm) { return alarm.state; }

    static void setState(OswAppAlarm &alarm, OswAppAlarm::AlarmState state) { alarm.state = state; }

    static std::array<unsigned char, 4> getTimestamp(OswAppAlarm &alarm) { return alarm.timestamp; }

    static void setTimestamp(OswAppAlarm &alarm, std::array<unsigned char, 4> timestamp) { alarm.timestamp = timestamp; }

    static std::array<bool, 7> getDaysOfWeek(OswAppAlarm &alarm) { return alarm.daysOfWeek; }

    static void setDaysOfWeek(OswAppAlarm &alarm, std::array<bool, 7> daysOfWeek) { alarm.daysOfWeek = daysOfWeek; }

    static unsigned char getStep(OswAppAlarm &alarm) { return alarm.step; }

    static void setStep(OswAppAlarm &alarm, unsigned char step) { alarm.step = step; }

    static void reset(OswAppAlarm &alarm) { alarm.resetAlarmState(); }
};