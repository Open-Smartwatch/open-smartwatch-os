#include "utest.h"

#include "../Helpers/TestAlarm.h"

// Helpers
OswAppAlarm createAlarm()
{
    std::shared_ptr<OswAppSwitcher> mockSwitcher = std::make_shared<OswAppSwitcher>(BUTTON_1, SHORT_PRESS, false, false, nullptr);
    OswAppAlarm alarm{mockSwitcher.get()};
    return alarm;
}

// Tests
UTEST(alarm, initial_state_should_be_idle)
{
    auto alarm = createAlarm();
    int currentState = static_cast<int>(TestAlarm::getState(alarm));
    int idleState = static_cast<int>(OswAppAlarm::AlarmState::IDLE);

    EXPECT_EQ(currentState, idleState);
}

UTEST(alarm, initial_timestamp_should_be_zeros)
{
    auto alarm = createAlarm();
    auto timestamp = TestAlarm::getTimestamp(alarm);

    for (unsigned char digit : timestamp)
    {
        EXPECT_EQ(digit, 0);
    }
}

UTEST(alarm, initial_days_selection_should_be_false)
{
    auto alarm = createAlarm();
    auto daysOfWeek = TestAlarm::getDaysOfWeek(alarm);

    for (bool day : daysOfWeek)
    {
        EXPECT_EQ(day, false);
    }
}

UTEST(alarm, should_reset)
{
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
    for (unsigned char digit : timestamp)
    {
        ASSERT_EQ(digit, 0);
    }
    for (bool day : daysOfWeek)
    {
        ASSERT_EQ(day, false);
    }
}