#ifndef OSW_SERVICE_TASKNOTIFIER_H
#define OSW_SERVICE_TASKNOTIFIER_H

#include "../lib/date/include/date/date.h"

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

    std::pair<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>, Notification> createNotification(
        const std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> timeToFire, const std::string publisher,
        const std::string message = "", const std::array<bool, 7> daysOfWeek = std::array<bool, 7>{});

    std::pair<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>, Notification> createNotification(
        const int hours, const int minutes, const std::string publisher,
        const std::string message = "", const std::array<bool, 7> daysOfWeek = std::array<bool, 7>{});

    std::vector<std::pair<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>, Notification>> readNotifications(const std::string publisher);

    void deleteNotification(const unsigned id, const std::string publisher);

    std::multimap<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>, const Notification> scheduler{};

    std::mutex mutlimapMutex{};

    // For testing purposes (to access and test private members)
    friend class TestOswServiceTaskNotifier;
};

#endif