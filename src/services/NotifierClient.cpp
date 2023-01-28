#include "./services/NotifierClient.h"

NotifierClient::NotifierClient(std::string publisher) : publisher{std::move(publisher)} {
}

std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> getCurrentTime()
{
    auto utcTime = std::chrono::system_clock::from_time_t(OswHal::getInstance()->getUTCTime());
    auto currentTime = utcTime + std::chrono::seconds{static_cast<int>(OswHal::getInstance()->getTimezoneOffsetPrimary())};
    return std::chrono::time_point_cast<std::chrono::seconds>(currentTime);
}

NotificationData NotifierClient::createNotification(std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> timeToFire,
        std::string message, std::array<bool, 7> daysOfWeek, bool isPersistent) {
    return OswServiceAllTasks::notifier.createNotification(timeToFire, getCurrentTime(), publisher, std::move(message), std::move(daysOfWeek), isPersistent);
}

NotificationData NotifierClient::createNotification(int hours, int minutes,
        std::string message, std::array<bool, 7> daysOfWeek, bool isPersistent) {
    return OswServiceAllTasks::notifier.createNotification(hours, minutes, getCurrentTime(), publisher, std::move(message), std::move(daysOfWeek), isPersistent);
}

std::vector<NotificationData> NotifierClient::readNotifications() {
    return OswServiceAllTasks::notifier.readNotifications(publisher);
}

void NotifierClient::deleteNotification(int64_t id) {
    OswServiceAllTasks::notifier.deleteNotification(id, publisher);
}