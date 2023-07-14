#include "utest.h"

#include "../../../include/apps/clock/OswAppTimer.h"

// This is a friend class of OswAppTimer. It is needed to access and test private members of OswAppTimer
class TestTimer {
  public:
    static OswAppTimer::TimerState getState(OswAppTimer& timer) {
        return timer.state;
    }

    static void setState(OswAppTimer& timer, OswAppTimer::TimerState state) {
        timer.state = state;
    }

    static std::array<unsigned char, 6> getTimestamp(OswAppTimer& timer) {
        return timer.timestamp;
    }

    static void setTimestamp(OswAppTimer& timer, std::array<unsigned char, 6> timestamp) {
        timer.timestamp = timestamp;
    }

    static void timestampToSec(OswAppTimer& timer) {
        timer.timestampToSec();
    }

    static std::chrono::seconds getLeftSec(OswAppTimer& timer) {
        return timer.timerLeftSec;
    }

    static unsigned char getStep(OswAppTimer& timer) {
        return timer.step;
    }

    static void setStep(OswAppTimer& timer, unsigned char step) {
        timer.step = step;
    }

    static void reset(OswAppTimer& timer) {
        timer.resetTimer();
    }
};

// Helpers
static OswAppTimer createTimer() {
    OswAppTimer timer;
    return timer;
}

// Tests
UTEST(timer, initial_state_should_be_idle) {
    auto timer = createTimer();
    int currentState = static_cast<int>(TestTimer::getState(timer));
    int idleState = static_cast<int>(OswAppTimer::TimerState::IDLE);

    EXPECT_EQ(currentState, idleState);
}

UTEST(timer, initial_timestamp_should_be_zeros) {
    auto timer = createTimer();
    auto timestamp = TestTimer::getTimestamp(timer);

    for (unsigned char digit : timestamp) {
        EXPECT_EQ(digit, 0);
    }
}

UTEST(timer, timestamp_seconds_should_parse_to_seconds) {
    auto timer = createTimer();

    TestTimer::setTimestamp(timer, {0, 0, 0, 0, 1, 5}); // [00:00:15]
    TestTimer::timestampToSec(timer);
    int timerLeftSec = TestTimer::getLeftSec(timer).count();

    EXPECT_EQ(timerLeftSec, 15);
}

UTEST(timer, timestamp_minutes_should_parse_to_seconds) {
    auto timer = createTimer();

    TestTimer::setTimestamp(timer, {0, 0, 1, 5, 0, 0}); // [00:15:00]
    TestTimer::timestampToSec(timer);
    int timerLeftSec = TestTimer::getLeftSec(timer).count();

    EXPECT_EQ(timerLeftSec, 15 * 60);
}

UTEST(timer, timestamp_hours_should_parse_to_seconds) {
    auto timer = createTimer();

    TestTimer::setTimestamp(timer, {1, 5, 0, 0, 0, 0}); // [15:00:00]
    TestTimer::timestampToSec(timer);
    int timerLeftSec = TestTimer::getLeftSec(timer).count();

    EXPECT_EQ(timerLeftSec, 15 * 60 * 60);
}

UTEST(timer, should_reset) {
    auto timer = createTimer();

    // Set some mock values
    TestTimer::setState(timer, OswAppTimer::TimerState::RUNNING);
    TestTimer::setStep(timer, 5);
    TestTimer::setTimestamp(timer, {1, 1, 1, 1, 1, 1});
    TestTimer::timestampToSec(timer);

    // Reset
    TestTimer::reset(timer);

    int currentState = static_cast<int>(TestTimer::getState(timer));
    unsigned char step = TestTimer::getStep(timer);
    auto timestamp = TestTimer::getTimestamp(timer);
    int timerLeftSec = TestTimer::getLeftSec(timer).count();

    ASSERT_EQ(currentState, 0);
    ASSERT_EQ(step, 0);
    for (unsigned char digit : timestamp) {
        ASSERT_EQ(digit, 0);
    }
    ASSERT_EQ(timerLeftSec, 0);
}