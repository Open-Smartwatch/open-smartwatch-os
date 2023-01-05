#ifndef OSW_SERVICE_TASKNOTIFIERCLIENT_H
#define OSW_SERVICE_TASKNOTIFIERCLIENT_H

#include "./services/OswServiceTasks.h"
#include "./services/OswServiceTaskNotifier.h"

#include <string>

class NotifierClient
{
public:
    NotifierClient(std::string publisher);

    NotificationData createNotification(std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> timeToFire,
                                        std::string message = "", std::array<bool, 7> daysOfWeek = std::array<bool, 7>{});

    NotificationData createNotification(int hours, int minutes,
                                        std::string message = "", std::array<bool, 7> daysOfWeek = std::array<bool, 7>{});

    std::vector<NotificationData> readNotifications();

    void deleteNotification(unsigned id);

private:
    const std::string publisher{};
};

#endif