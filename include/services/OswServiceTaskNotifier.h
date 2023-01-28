#ifndef OSW_SERVICE_TASKNOTIFIER_H
#define OSW_SERVICE_TASKNOTIFIER_H

#include <algorithm>
#include <map>

#include "../lib/date/date.h"
#include "Notification.h"
#include "osw_service.h"
#include "osw_ui.h"

class OswServiceTaskNotifier : public OswServiceTask {
public:
    OswServiceTaskNotifier() {};
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswServiceTaskNotifier() {};

private:
    friend class NotifierClient;

    NotificationData createNotification(std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> timeToFire,
                                        std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> timeCreated,
                                        std::string publisher, std::string message = {}, std::array<bool, 7> daysOfWeek = {}, bool isPersistent = {});

    NotificationData createNotification(int hours, int minutes, std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> timeCreated,
                                        std::string publisher, std::string message = {}, std::array<bool, 7> daysOfWeek = {}, bool isPersistent = {});

    std::vector<NotificationData> readNotifications(std::string publisher);

    void deleteNotification(int64_t id, std::string publisher);

    std::multimap<NotificationData::first_type, const NotificationData::second_type> scheduler{};

    std::mutex mutlimapMutex{};

    // For testing purposes (to access and test private members)
    friend class TestOswServiceTaskNotifier;
};

#endif