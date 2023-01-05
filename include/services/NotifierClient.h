#ifndef OSW_SERVICE_TASKNOTIFIERCLIENT_H
#define OSW_SERVICE_TASKNOTIFIERCLIENT_H

#include "./services/OswServiceTasks.h"
#include "./services/OswServiceTaskNotifier.h"

#include <string>

class NotifierClient
{
public:
    NotifierClient(const std::string publisher);

    std::pair<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>, Notification> createNotification(
        const std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> timeToFire,
        const std::string message = "", const std::array<bool, 7> daysOfWeek = std::array<bool, 7>{});

    std::pair<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>, Notification> createNotification(
        const int hours, const int minutes,
        const std::string message = "", const std::array<bool, 7> daysOfWeek = std::array<bool, 7>{});

    std::vector<std::pair<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>, Notification>> readNotifications();

    void deleteNotification(const unsigned id);

private:
    const std::string publisher = "";
};

#endif