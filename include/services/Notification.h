#ifndef H_NOTIFICATION
#define H_NOTIFICATION

#include <array>
#include <chrono>
#include <string>

class Notification {
public:
    Notification(std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> timeCreated,
                 std::string publisher, std::string message = {}, std::array<bool, 7> daysOfWeek = {}, bool isPersistent = {})
        : timeCreated{timeCreated}, publisher{std::move(publisher)}, message{std::move(message)}, daysOfWeek{std::move(daysOfWeek)}, isPersistent{isPersistent} {}

    int64_t getId() const {
        return timeCreated.time_since_epoch().count();
    }

    std::string getPublisher() const {
        return publisher;
    }

    std::string getMessage() const {
        return message;
    }

    std::array<bool, 7> getDaysOfWeek() const {
        return daysOfWeek;
    }

    bool getPersistence() const {
        return isPersistent;
    }

private:
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> timeCreated{};
    const std::string publisher{};
    const std::string message{};
    const std::array<bool, 7> daysOfWeek{};
    const bool isPersistent{};
};

typedef std::pair<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>, Notification> NotificationData;

#endif