#include "utest.h"

#include "../../../include/apps/clock/OswAppAlarm.h"

// This is a friend class of OswAppAlarm. It is needed to access and test private members of OswAppAlarm
class TestAlarm {
  public:
    static OswAppAlarm::AlarmState getState(OswAppAlarm& alarm) {
        return alarm.state;
    }

    static void setState(OswAppAlarm& alarm, OswAppAlarm::AlarmState state) {
        alarm.state = state;
    }

    static std::array<unsigned char, 4> getTimestamp(OswAppAlarm& alarm) {
        return alarm.timestamp;
    }

    static void setTimestamp(OswAppAlarm& alarm, std::array<unsigned char, 4> timestamp) {
        alarm.timestamp = timestamp;
    }

    static std::array<bool, 7> getDaysOfWeek(OswAppAlarm& alarm) {
        return alarm.daysOfWeek;
    }

    static void setDaysOfWeek(OswAppAlarm& alarm, std::array<bool, 7> daysOfWeek) {
        alarm.daysOfWeek = daysOfWeek;
    }

    static unsigned char getStep(OswAppAlarm& alarm) {
        return alarm.step;
    }

    static void setStep(OswAppAlarm& alarm, unsigned char step) {
        alarm.step = step;
    }

    static void reset(OswAppAlarm& alarm) {
        alarm.resetAlarmState();
    }
};

// Helpers
static OswAppAlarm createAlarm() {
    OswAppAlarm alarm;
    return alarm;
}

// Tests
UTEST(alarm, initial_state_should_be_idle) {
    auto alarm = createAlarm();
    int currentState = static_cast<int>(TestAlarm::getState(alarm));
    int idleState = static_cast<int>(OswAppAlarm::AlarmState::IDLE);

    EXPECT_EQ(currentState, idleState);
}

UTEST(alarm, initial_timestamp_should_be_zeros) {
    auto alarm = createAlarm();
    auto timestamp = TestAlarm::getTimestamp(alarm);

    for (unsigned char digit : timestamp) {
        EXPECT_EQ(digit, 0);
    }
}

UTEST(alarm, initial_days_selection_should_be_false) {
    auto alarm = createAlarm();
    auto daysOfWeek = TestAlarm::getDaysOfWeek(alarm);

    for (bool day : daysOfWeek) {
        EXPECT_EQ(day, false);
    }
}

UTEST(alarm, should_reset) {
    auto alarm = createAlarm();

    // Set some mock values
    TestAlarm::setState(alarm, OswAppAlarm::AlarmState::DAY_PICKER);
    TestAlarm::setStep(alarm, 5);
    TestAlarm::setTimestamp(alarm, {1, 1, 1, 1});
    TestAlarm::setDaysOfWeek(alarm, {true, true, true, true, true, true, true});

    // Reset
    TestAlarm::reset(alarm);

    // Get updated values
    int currentState = static_cast<int>(TestAlarm::getState(alarm));
    unsigned char step = TestAlarm::getStep(alarm);
    auto timestamp = TestAlarm::getTimestamp(alarm);
    auto daysOfWeek = TestAlarm::getDaysOfWeek(alarm);

    ASSERT_EQ(currentState, 0);
    ASSERT_EQ(step, 0);
    for (unsigned char digit : timestamp) {
        ASSERT_EQ(digit, 0);
    }
    for (bool day : daysOfWeek) {
        ASSERT_EQ(day, false);
    }
}