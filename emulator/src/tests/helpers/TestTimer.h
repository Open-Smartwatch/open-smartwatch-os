#pragma once

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