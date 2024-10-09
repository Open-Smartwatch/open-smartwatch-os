#include "./services/NotifierClient.h"

NotifierClient::NotifierClient(std::string publisher) : publisher{std::move(publisher)} {
}

NotificationData NotifierClient::createNotification(std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> timeToFire,
        std::string message, std::array<bool, 7> daysOfWeek, bool isPersistent) {
    return OswServiceAllTasks::notifier.createNotification(timeToFire, publisher, std::move(message), std::move(daysOfWeek), isPersistent);
}

NotificationData NotifierClient::createNotification(int hours, int minutes,
        std::string message, std::array<bool, 7> daysOfWeek, bool isPersistent) {
    return OswServiceAllTasks::notifier.createNotification(hours, minutes, publisher, std::move(message), std::move(daysOfWeek), isPersistent);
}

std::vector<NotificationData> NotifierClient::readNotifications() {
    return OswServiceAllTasks::notifier.readNotifications(publisher);
}

void NotifierClient::deleteNotification(unsigned id) {
    OswServiceAllTasks::notifier.deleteNotification(id, publisher);
}
