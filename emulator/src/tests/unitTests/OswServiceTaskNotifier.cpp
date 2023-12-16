#include "utest.h"

#include "../../../include/services/OswServiceTaskNotifier.h"

using namespace std::chrono;

// This is a friend class of OswServiceTaskNotifier. It is needed to access and test private members of OswServiceTaskNotifier
class TestOswServiceTaskNotifier {
  public:
    struct CreateNotificationArgs {
        const std::string publisher;
        const std::string message = "";
        const std::array<bool, 7> daysOfWeek{};
    };

    static NotificationData createNotification(
        const time_point<system_clock, seconds> timeToFire, CreateNotificationArgs args, OswServiceTaskNotifier& notifier) {
        return notifier.createNotification(timeToFire, args.publisher, args.message, args.daysOfWeek);
    }

    // TODO: Add unit test for this method
    static NotificationData createNotification(
        const int hours, const int minutes, CreateNotificationArgs args, OswServiceTaskNotifier& notifier) {
        return notifier.createNotification(hours, minutes, args.publisher, args.message, args.daysOfWeek);
    }

    static std::vector<NotificationData> readNotifications(
        const std::string publisher, OswServiceTaskNotifier& notifier) {
        return notifier.readNotifications(publisher);
    }

    static void deleteNotification(unsigned int id, const std::string publisher, OswServiceTaskNotifier& notifier) {
        notifier.deleteNotification(id, publisher);
    }

    static std::multimap<time_point<system_clock, seconds>, const Notification> getScheduler(OswServiceTaskNotifier& notifier) {
        return notifier.scheduler;
    }
};

// Helpers
static NotificationData addNotification(OswServiceTaskNotifier& notifier, int mockTime_Sec = 42, std::string publisher = "mock-publisher") {
    time_point<system_clock, seconds> timeToFire_Sec{duration(seconds(mockTime_Sec))};

    TestOswServiceTaskNotifier::CreateNotificationArgs args{publisher, "", std::array<bool, 7>{}};

    return TestOswServiceTaskNotifier::createNotification(timeToFire_Sec, args, notifier);
}

// Tests
UTEST(notifier, should_add_notification_to_scheduler) {
    OswServiceTaskNotifier notifier{};

    addNotification(notifier);
    auto scheduler = TestOswServiceTaskNotifier::getScheduler(notifier);

    EXPECT_EQ(scheduler.size(), 1U);
}

UTEST(notifier, should_set_correct_time_to_notification) {
    OswServiceTaskNotifier notifier{};

    const int mockTime_Sec = 42;
    addNotification(notifier, mockTime_Sec);
    auto scheduler = TestOswServiceTaskNotifier::getScheduler(notifier);
    auto notificationTime_Sec = (*(scheduler.begin())).first.time_since_epoch().count();

    EXPECT_EQ(notificationTime_Sec, mockTime_Sec);
}

UTEST(notifier, should_return_all_notifications_of_specified_publisher) {
    OswServiceTaskNotifier notifier{};
    const int mockTime_Sec = 42;
    const std::string mockAlarmPublisher = "mock-alarm-publisher";
    const std::string mockTimerPublisher = "mock-timer-publisher";
    addNotification(notifier, mockTime_Sec, mockAlarmPublisher);
    addNotification(notifier, mockTime_Sec, mockAlarmPublisher);
    addNotification(notifier, mockTime_Sec, mockAlarmPublisher);
    addNotification(notifier, mockTime_Sec, mockTimerPublisher);
    addNotification(notifier, mockTime_Sec, mockTimerPublisher);

    auto scheduler = TestOswServiceTaskNotifier::getScheduler(notifier);
    auto notificationPublisher = (*(scheduler.begin())).second.getPublisher();
    auto alarmNotifications = TestOswServiceTaskNotifier::readNotifications(mockAlarmPublisher, notifier);

    EXPECT_EQ(alarmNotifications.size(), 3U);
    for (auto notification : alarmNotifications) {
        auto notificationTime_Sec = notification.first.time_since_epoch().count();
        EXPECT_EQ(notificationTime_Sec, mockTime_Sec);
    }
}

UTEST(notifier, should_delete_specified_notification) {
    OswServiceTaskNotifier notifier{};
    const int mockTime_Sec = 42;
    const std::string mockAlarmPublisher = "mock-alarm-publisher";
    const std::string mockTimerPublisher = "mock-timer-publisher";
    addNotification(notifier, mockTime_Sec + 1, mockAlarmPublisher);
    auto notificationToDelete = addNotification(notifier, mockTime_Sec, mockAlarmPublisher);
    addNotification(notifier, mockTime_Sec + 1, mockAlarmPublisher);
    addNotification(notifier, mockTime_Sec, mockTimerPublisher);
    addNotification(notifier, mockTime_Sec, mockTimerPublisher);

    TestOswServiceTaskNotifier::deleteNotification(notificationToDelete.second.getId(), notificationToDelete.second.getPublisher(), notifier);
    auto scheduler = TestOswServiceTaskNotifier::getScheduler(notifier);
    auto alarmNotifications = TestOswServiceTaskNotifier::readNotifications(mockAlarmPublisher, notifier);

    ASSERT_EQ(scheduler.size(), 4U);
    ASSERT_EQ(alarmNotifications.size(), 2U);
    for (auto notification : alarmNotifications) {
        auto notificationTime_Sec = notification.first.time_since_epoch().count();
        ASSERT_EQ(notificationTime_Sec, mockTime_Sec + 1);
    }
}