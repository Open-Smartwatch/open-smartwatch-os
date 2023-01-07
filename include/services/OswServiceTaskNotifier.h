#ifndef OSW_SERVICE_TASKNOTIFIER_H
#define OSW_SERVICE_TASKNOTIFIER_H

#include "../lib/date/date.h"

#include "osw_service.h"

#include <algorithm>
#include <map>

class Notification
{
public:
    unsigned getId() const
    {
        return id;
    }

    std::string getMessage() const
    {
        return message;
    }

    std::string getPublisher() const
    {
        return publisher;
    }

    std::array<bool, 7> getDaysOfWeek() const
    {
        return daysOfWeek;
    }

private:
    friend class OswServiceTaskNotifier;

    Notification(std::string publisher, std::string message = "", std::array<bool, 7> daysOfWeek = std::array<bool, 7>{})
        : publisher{publisher}, message{message}, daysOfWeek{daysOfWeek}
    {
        id = count;
        ++count;
    }

    unsigned id{};
    static unsigned count;
    std::string publisher{};
    std::string message{};
    std::array<bool, 7> daysOfWeek{};
};

typedef std::pair<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>, Notification> NotificationData;

class OswServiceTaskNotifier : public OswServiceTask
{
public:
    OswServiceTaskNotifier(){};
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswServiceTaskNotifier(){};

private:
    friend class NotifierClient;

    NotificationData createNotification(std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> timeToFire, std::string publisher,
                                        std::string message = "", std::array<bool, 7> daysOfWeek = std::array<bool, 7>{});

    NotificationData createNotification(int hours, int minutes, std::string publisher,
                                        std::string message = "", std::array<bool, 7> daysOfWeek = std::array<bool, 7>{});

    std::vector<NotificationData> readNotifications(std::string publisher);

    void deleteNotification(unsigned id, std::string publisher);

    std::multimap<NotificationData::first_type, const NotificationData::second_type> scheduler{};

    std::mutex mutlimapMutex{};

    // For testing purposes (to access and test private members)
    friend class TestOswServiceTaskNotifier;
};

#endif