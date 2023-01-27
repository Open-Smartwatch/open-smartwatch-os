#pragma once

#include <memory>

#include <osw_hal.h>
#include <devices/interfaces/OswTemperatureProvider.h>
#include <devices/interfaces/OswAccelerationProvider.h>
#include <devices/interfaces/OswMagnetometerProvider.h>
#include <devices/interfaces/OswHumidityProvider.h>
#include <devices/interfaces/OswPressureProvider.h>

#if OSW_SERVICE_NOTIFIER == 1
#define MESSAGE_LENGTH 50

#include <map>
#include "./services/Notification.h"

struct NotificationDto {
    time_t timeToFire;
    time_t id;
    char message[MESSAGE_LENGTH];
    char publisher[MESSAGE_LENGTH];
    bool daysOfWeek[7];
    bool isPersistent;
};
#endif

#if OSW_PLATFORM_ENVIRONMENT == 1
class OswHal::Environment {
  public:
    void updateProviders(); // Retrieve all environment sensors
    void reset(); // Reset all data on environment sensors

#if OSW_PLATFORM_ENVIRONMENT_TEMPERATURE == 1
    float getTemperature();
#endif

#if OSW_PLATFORM_ENVIRONMENT_PRESSURE == 1
    float getPressure();
#endif

#if OSW_PLATFORM_ENVIRONMENT_HUMIDITY == 1
    float getHumidity();
#endif

#if OSW_PLATFORM_ENVIRONMENT_MAGNETOMETER == 1
    int getMagnetometerAzimuth();
#endif

#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
    float getAccelerationX();
    float getAccelerationY();
    float getAccelerationZ();
    // Statistics: Steps
    uint32_t getStepsToday();
    void resetStepCount();
    uint32_t getStepsTotal();
    uint32_t getStepsTotalWeek();
#ifdef OSW_FEATURE_STATS_STEPS
    void setupStepStatistics();
    uint32_t getStepsAverage();
    uint32_t getStepsOnDay(uint8_t dayOfWeek, bool lastWeek = false);
  private:
    void commitStepStatistics(const bool& alwaysPrintStepStatistics = false);
  public: // This public is here, just in case someone will add further public-functions after this private one...
#endif
#endif

#if OSW_SERVICE_NOTIFIER == 1
    void setupNotifications();
    std::multimap<NotificationData::first_type, const NotificationData::second_type> getNotifications();
    void commitNotifications(std::multimap<NotificationData::first_type, const NotificationData::second_type> notifications);
#endif

  protected:
    Environment() {}
    ~Environment() {}
    friend OswHal;
    friend std::unique_ptr<OswHal::Environment>::deleter_type;
  private:
#ifdef OSW_FEATURE_STATS_STEPS
    uint32_t _stepsCache[7] = {0};
    uint32_t _stepsSum = 0;
    uint32_t _stepsLastDoW = 0;
#endif

#if OSW_SERVICE_NOTIFIER == 1
    static constexpr size_t _notifCount = 10;
    NotificationDto _notifs[_notifCount]{};
#endif

    OswTemperatureProvider* tempSensor = nullptr;
    OswAccelerationProvider* accelSensor = nullptr;
    OswMagnetometerProvider* magSensor = nullptr;
    OswHumidityProvider* humiSensor = nullptr;
    OswPressureProvider* pressSensor = nullptr;
};
#endif