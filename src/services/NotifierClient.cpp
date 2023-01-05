#include "./services/NotifierClient.h"

NotifierClient::NotifierClient(const std::string publisher) : publisher{publisher}
{
}

std::pair<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>, Notification> NotifierClient::createNotification(
    const std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> timeToFire,
    const std::string message, const std::array<bool, 7> daysOfWeek)
{
    return OswServiceAllTasks::notifier.createNotification(timeToFire, publisher, message, daysOfWeek);
}

std::pair<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>, Notification> NotifierClient::createNotification(
    const int hours, const int minutes,
    const std::string message, const std::array<bool, 7> daysOfWeek)
{
    return OswServiceAllTasks::notifier.createNotification(hours, minutes, publisher, message, daysOfWeek);
}

std::vector<std::pair<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>, Notification>> NotifierClient::readNotifications()
{
    return OswServiceAllTasks::notifier.readNotifications(publisher);
}

void NotifierClient::deleteNotification(const unsigned id)
{
    OswServiceAllTasks::notifier.deleteNotification(id, publisher);
}