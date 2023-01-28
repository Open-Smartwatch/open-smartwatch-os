#ifndef OSW_APP_TIMER_H
#define OSW_APP_TIMER_H

#include "../lib/date/date.h"

#include "osw_app.h"
#include <osw_hal.h>
#include <osw_ui.h>
#include "apps/main/switcher.h"
#include "./services/NotifierClient.h"

#include <array>

class OswAppTimer : public OswApp
{
public:
    enum class TimerState
    {
        IDLE,
        SET_TIMER_SCREEN,
        RUNNING,
        PAUSED
    };

    OswAppTimer(OswAppSwitcher *clockAppSwitcher);
    void setup() override;
    void loop() override;
    void stop() override;
    ~OswAppTimer(){};

private:
    // Handle buttons
    void handleNextButton(const unsigned char optionsCount);
    void handleIncrementButton();
    void handleDecrementButton();
    long handleResetButton();

    // Helpers
    void timestampToSec();
    void resetTimer();

    // Draw on screen
    void drawNumber(const int number, const int index);
    void drawTime(const int totalSeconds);

    OswAppSwitcher *clockAppSwitcher{};
    NotifierClient notifierClient{"com.akmal-threepointsix.application.timer"};
    TimerState state{};
    unsigned char step{};
    std::array<unsigned char, 6> timestamp{};
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> timeToFire{};
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> timerPauseTime{};
    std::chrono::seconds timerLeftSec{};
    int64_t timerId{};

    // For testing purposes
    friend class TestTimer;
};

#endif
