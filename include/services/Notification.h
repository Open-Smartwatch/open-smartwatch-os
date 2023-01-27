#ifndef H_NOTIFICATION
#define H_NOTIFICATION

#include <array>
#include <chrono>
#include <string>

class Notification {
  public:
    Notification(std::string publisher, std::string message = {}, std::array<bool, 7> daysOfWeek = {}, bool isPersistent = {})
        : publisher{std::move(publisher)}, message{std::move(message)}, daysOfWeek{std::move(daysOfWeek)}, isPersistent{isPersistent} {
        id = count;
        ++count;
    }

    unsigned getId() const {
        return id;
    }

    std::string getMessage() const {
        return message;
    }

    std::string getPublisher() const {
        return publisher;
    }

    std::array<bool, 7> getDaysOfWeek() const {
        return daysOfWeek;
    }

    bool getPersistence() const {
        return isPersistent;
    }

  private:
    unsigned id{};
    static unsigned count;
    const std::string publisher{};
    const std::string message{};
    const std::array<bool, 7> daysOfWeek{};
    const bool isPersistent{};
};

typedef std::pair<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>, Notification> NotificationData;

#endif