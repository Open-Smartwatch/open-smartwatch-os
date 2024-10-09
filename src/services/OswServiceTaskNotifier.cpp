#include "./services/OswServiceTaskNotifier.h"

unsigned Notification::count = 0;

NotificationData OswServiceTaskNotifier::createNotification(std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> timeToFire, std::string publisher,
        std::string message, std::array<bool, 7> daysOfWeek, bool isPersistent) {
    const std::lock_guard<std::mutex> lock{mutlimapMutex};
    auto notification = Notification{std::move(publisher), std::move(message), std::move(daysOfWeek), isPersistent};
    auto pair = std::make_pair(timeToFire, notification);
    scheduler.insert(pair);
    return pair;
}

std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> getTimeToFire(int hours, int minutes) {
    auto utcTime = std::chrono::system_clock::from_time_t(OswHal::getInstance()->getUTCTime());
    auto currentTime = utcTime + std::chrono::seconds{static_cast<int>(OswHal::getInstance()->getTimezoneOffsetPrimary())};
    auto currentDate = date::sys_days(floor<date::days>(currentTime));
    auto scheduledTime = std::chrono::hours{hours} + std::chrono::minutes{minutes} + std::chrono::seconds{0};
    auto timeToFire = currentDate + scheduledTime;
    if (currentTime >= timeToFire) {
        timeToFire += date::days{1};
    }
    return timeToFire;
}

// At least one day of week must be selected!
std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> getTimeToFire(int hours, int minutes, std::array<bool, 7> daysOfWeek) {
    auto timeToFire = getTimeToFire(hours, minutes);
    auto scheduledDays = date::sys_days{floor<date::days>(timeToFire)};
    auto dayOfWeek = date::weekday{scheduledDays};
    auto i = dayOfWeek.c_encoding();
    while (!daysOfWeek[i]) {
        timeToFire += date::days{1};
        i = (i + 1) % daysOfWeek.size();
    }
    return timeToFire;
}

NotificationData OswServiceTaskNotifier::createNotification(int hours, int minutes, std::string publisher,
        std::string message, std::array<bool, 7> daysOfWeek, bool isPersistent) {
    const std::lock_guard<std::mutex> lock{mutlimapMutex};
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> timeToFire{};
    auto notification = Notification{std::move(publisher), std::move(message), std::move(daysOfWeek), isPersistent};
    if (std::any_of(daysOfWeek.begin(), daysOfWeek.end(), [](auto x) {
    return x;
})) {
        timeToFire = getTimeToFire(hours, minutes, daysOfWeek);
    } else {
        timeToFire = getTimeToFire(hours, minutes);
    }
    auto pair = std::make_pair(timeToFire, notification);
    scheduler.insert(pair);
    return pair;
}

std::vector<NotificationData> OswServiceTaskNotifier::readNotifications(const std::string publisher) {
    const std::lock_guard<std::mutex> lock{mutlimapMutex};
    std::vector<NotificationData> result{};
    result.reserve(scheduler.size());
    for (auto it = scheduler.begin(); it != scheduler.end(); ++it) {
        if (it->second.getPublisher() == publisher) {
            result.push_back({it->first, it->second});
        }
    }
    return result;
}

void OswServiceTaskNotifier::deleteNotification(const unsigned id, const std::string publisher) {
    const std::lock_guard<std::mutex> lock{mutlimapMutex};
    for (auto it = scheduler.begin(); it != scheduler.end(); ++it) {
        if (it->second.getId() == id && it->second.getPublisher() == publisher) {
            scheduler.erase(it);
            return;
        }
    }
}

void OswServiceTaskNotifier::setup() {
    OswServiceTask::setup();
}

void OswServiceTaskNotifier::loop() {
    const std::lock_guard<std::mutex> lock{mutlimapMutex};
    auto utcTime = std::chrono::system_clock::from_time_t(OswHal::getInstance()->getUTCTime());
    auto currentTime = utcTime + std::chrono::seconds{static_cast<int>(OswHal::getInstance()->getTimezoneOffsetPrimary())};
    if (auto it = scheduler.begin();
            it != scheduler.end() && currentTime >= it->first) {
        auto timeToFire = it->first;
        const auto& notification = it->second;
#ifdef OSW_EMULATOR
        OSW_LOG_D("Fired a notification");
        auto t = std::chrono::system_clock::to_time_t(timeToFire);
        OSW_LOG_D(std::put_time(std::localtime(&t), "%F %T.\n"));
        OSW_LOG_D(notification.getMessage());
#endif
        OswUI::getInstance()->showNotification(std::move(notification.getMessage()), notification.getPersistence());
        auto daysOfWeek = notification.getDaysOfWeek();
        if (std::any_of(daysOfWeek.begin(), daysOfWeek.end(), [](auto x) {
        return x;
    })) {
            date::hh_mm_ss time{floor<std::chrono::seconds>(timeToFire - floor<date::days>(timeToFire))};
            timeToFire = getTimeToFire(time.hours().count(), time.minutes().count(), daysOfWeek);
            scheduler.insert({timeToFire, Notification{notification.getPublisher(), notification.getMessage(), notification.getDaysOfWeek()}});
        }
        scheduler.erase(it);
    }
}

void OswServiceTaskNotifier::stop() {
    OswServiceTask::stop();
}
