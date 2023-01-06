#ifndef OWS_APP_ALARM_H
#define OSW_APP_ALARM_H

#include "../lib/date/include/date/date.h"

#include <osw_app.h>
#include <osw_hal.h>
#include <osw_ui.h>
#include "apps/main/switcher.h"
#include "./services/NotifierClient.h"

#include <array>
#include <vector>
#include <algorithm>

class OswAppAlarm : public OswApp
{
public:
    enum class AlarmState
    {
        IDLE,
        LIST,
        TIME_PICKER,
        DAY_PICKER
    };

    OswAppAlarm(OswAppSwitcher *clockAppSwitcher);
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppAlarm() {}

private:
    void handleNextButton(const unsigned char optionsCount);
    void handleTimeIncrementButton();
    void handleTimeDecrementButton();
    void handleFrequencyIncrementButton();
    void resetAlarmState();
    void listAlarms();

    OswAppSwitcher *clockAppSwitcher{};
    NotifierClient notifierClient{"com.cutieskye.application.alarm"};
    AlarmState state{};
    unsigned char step{};
    std::array<unsigned char, 4> timestamp{};
    std::array<bool, 7> daysOfWeek{};
    std::vector<NotificationData> notifications{};
    const size_t ALARM_COUNT = 4;

    // For testing purposes (to access and test private members)
    friend class TestAlarm;
};

#endif