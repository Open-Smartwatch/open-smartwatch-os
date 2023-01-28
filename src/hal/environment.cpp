#include <stdexcept>
#ifdef OSW_EMULATOR
#include <cassert>
#endif

#include <Preferences.h>
#include <osw_hal.h>

#define PREFS_STEPS "PS"
#define PREFS_STEPS_DAYLASTCHECKED "L"
#define PREFS_STEPS_STATS "S"
#define PREFS_STEPS_ALL "A"

#define PREFS_NOTIFS "PN"

void OswHal::Environment::updateProviders() {
    // In case we come from deepsleep (or whenever the available devices change), we should first scan the current available devices for possible providers...
#if OSW_PLATFORM_ENVIRONMENT_TEMPERATURE == 1
    for(auto& d : *OswTemperatureProvider::getAllTemperatureDevices()) {
        if(this->tempSensor == nullptr or this->tempSensor->getTemperatureProviderPriority() < d->getTemperatureProviderPriority())
            this->tempSensor = d;
    }
    if(!this->tempSensor)
        OSW_LOG_W("Temperature API enabled, but no provider available!");
#endif
#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
    for(auto& d : *OswAccelerationProvider::getAllAccelerationDevices()) {
        if(this->accelSensor == nullptr or this->accelSensor->getAccelerationProviderPriority() < d->getAccelerationProviderPriority())
            this->accelSensor = d;
    }
    if(!this->accelSensor)
        OSW_LOG_W("Acceleration API enabled, but no provider available!");
#endif
#if OSW_PLATFORM_ENVIRONMENT_MAGNETOMETER == 1
    for(auto& d : *OswMagnetometerProvider::getAllMagnetometerDevices()) {
        if(this->magSensor == nullptr or this->magSensor->getMagnetometerProviderPriority() < d->getMagnetometerProviderPriority())
            this->magSensor = d;
    }
    if(!this->magSensor)
        OSW_LOG_W("Magnetometer API enabled, but no provider available!");
#endif
#if OSW_PLATFORM_ENVIRONMENT_PRESSURE == 1
    for(auto& d : *OswPressureProvider::getAllPressureDevices()) {
        if(this->pressSensor == nullptr or this->pressSensor->getPressureProviderPriority() < d->getPressureProviderPriority())
            this->pressSensor = d;
    }
    if(!this->pressSensor)
        OSW_LOG_W("Pressure API enabled, but no provider available!");
#endif
#if OSW_PLATFORM_ENVIRONMENT_HUMIDITY == 1
    for(auto& d : *OswHumidityProvider::getAllHumidityDevices()) {
        if(this->humiSensor == nullptr or this->humiSensor->getHumidityProviderPriority() < d->getHumidityProviderPriority())
            this->humiSensor = d;
    }
    if(!this->humiSensor)
        OSW_LOG_W("Humidity API enabled, but no provider available!");
#endif
}

#if OSW_PLATFORM_ENVIRONMENT_TEMPERATURE == 1
float OswHal::Environment::getTemperature() {
    if(!this->tempSensor)
        throw std::runtime_error("No temperature provider!");
    return this->tempSensor->getTemperature();
}
#endif

#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
float OswHal::Environment::getAccelerationX() {
    if(!this->accelSensor)
        throw std::runtime_error("No acceleration provider!");
    return this->accelSensor->getAccelerationX();
}

float OswHal::Environment::getAccelerationY() {
    if(!this->accelSensor)
        throw std::runtime_error("No acceleration provider!");
    return this->accelSensor->getAccelerationY();
}

float OswHal::Environment::getAccelerationZ() {
    if(!this->accelSensor)
        throw std::runtime_error("No acceleration provider!");
    return this->accelSensor->getAccelerationZ();
}

uint32_t OswHal::Environment::getStepsToday() {
    if(!this->accelSensor)
        throw std::runtime_error("No acceleration provider!");
    return this->accelSensor->getStepCount();
}

void OswHal::Environment::resetStepCount() {
    if(!this->accelSensor)
        throw std::runtime_error("No acceleration provider!");
    return this->accelSensor->resetStepCount();
}

#ifdef OSW_FEATURE_STATS_STEPS
void OswHal::Environment::setupStepStatistics() {
    if(!this->accelSensor)
        throw std::runtime_error("No acceleration provider!");
    Preferences prefs;
    bool res = prefs.begin(PREFS_STEPS, false); // Open in RW mode, to allow creation as needed
    assert(res);
    if(prefs.getBytes(PREFS_STEPS_STATS, &this->_stepsCache, sizeof(this->_stepsCache)) != sizeof(this->_stepsCache)) {
        // Uoh, the steps history is not initialized -> fill it with zero and do it now!
        for(size_t i = 0; i < 7; i++)
            this->_stepsCache[i] = 0;
        res = prefs.putBytes(PREFS_STEPS_STATS, &this->_stepsCache, sizeof(this->_stepsCache)) == sizeof(this->_stepsCache);
        assert(res);
    } else {
        res = prefs.getBytes(PREFS_STEPS_STATS, &this->_stepsCache, sizeof(this->_stepsCache)) == sizeof(this->_stepsCache);
        assert(res);
    }
    this->_stepsSum = prefs.getUInt(PREFS_STEPS_ALL);
    this->_stepsLastDoW = prefs.getUInt(PREFS_STEPS_DAYLASTCHECKED);
    prefs.end(); // Make sure to close now, as the commit-function may open one instance itself
    this->commitStepStatistics(true);
}

/**
 * @brief This function will commit the history (and especially the current step count) to the history.
 * Call this if any user wants to interact with the history first. If no changes are needed, nothing will be done :)
 *
 * @param alwaysPrintStepStatistics Set to true to print the step history to the console
 */
void OswHal::Environment::commitStepStatistics(const bool& alwaysPrintStepStatistics) {
    uint32_t currDoM = 0; // Unused, but required by function signature
    uint32_t currDoW = 0;
    OswHal::getInstance()->getLocalDate(&currDoM, &currDoW);
    bool changedDoW = currDoW != this->_stepsLastDoW;
    if(changedDoW) {
        Preferences prefs;
        bool res = prefs.begin(PREFS_STEPS, false); // Open in RW, just in case
        const uint32_t currentSteps = this->getStepsToday();
        this->_stepsCache[this->_stepsLastDoW] = currentSteps; // write current step to last dow
        this->_stepsSum += currentSteps; // Let's just hope this never rolls over...
        OswHal::getInstance()->environment()->resetStepCount();
        if(OswConfigAllKeys::stepsHistoryClear.get()) {
            if(currDoW > this->_stepsLastDoW) {
                // set stepscache to 0 in ]lastDoW, currDoW[
                for(uint32_t i = currDoW; this->_stepsLastDoW + 1 < i; i--)
                    this->_stepsCache[i - 1] = 0;
            } else {
                // set > last dow to 0 && set < curr dow to 0
                if(currDoW > 0)
                    for(uint32_t i = currDoW; 0 < i; i--)
                        this->_stepsCache[i - 1] = 0;
                for(uint32_t i = this->_stepsLastDoW + 1; i < 7; i++)
                    this->_stepsCache[i] = 0;
            }
        }

        // Check if today is the initialization day
        short resetDay = OswConfigAllKeys::resetDay.get();
        if ((resetDay >= 0 && resetDay < 8) && (unsigned short) resetDay == currDoW + 1) // (e.g. 1 - 7 are days, 0 is disabled)
            this->_stepsSum = 0;

        this->_stepsLastDoW = currDoW;
        OSW_LOG_D("Updated steps from DoW ", this->_stepsLastDoW, " to DoW ", currDoW);

        // write step cache + stepsum
        res = prefs.putBytes(PREFS_STEPS_STATS, &this->_stepsCache, sizeof(this->_stepsCache)) == sizeof(this->_stepsCache);
        assert(res);
        res = prefs.putUInt(PREFS_STEPS_ALL, this->_stepsSum) == sizeof(this->_stepsSum);
        assert(res);
        res = prefs.putUInt(PREFS_STEPS_DAYLASTCHECKED, this->_stepsLastDoW) == sizeof(this->_stepsLastDoW);
        assert(res);
        prefs.end();
    }

#ifndef NDEBUG
    if(changedDoW or alwaysPrintStepStatistics) {
        String stepHistoryDbgMsg = "Current step history (day " + String(currDoW) + ", today " + String(OswHal::getInstance()->environment()->getStepsToday()) + ", sum " + String(this->_stepsSum) + ") is: {";
        for(size_t i = 0; i < 7; i++) {
            if(i > 0)
                stepHistoryDbgMsg += ", ";
            if(i == currDoW)
                stepHistoryDbgMsg += "[";
            stepHistoryDbgMsg += this->_stepsCache[i];
            if(i == currDoW)
                stepHistoryDbgMsg += "]";
        }
        stepHistoryDbgMsg += "}";
        OSW_LOG_D(stepHistoryDbgMsg);
    }
#endif
}

uint32_t OswHal::Environment::getStepsOnDay(uint8_t dayOfWeek, bool lastWeek) {
    this->commitStepStatistics();
    uint32_t day = 0;
    uint32_t weekday = 0;
    OswHal::getInstance()->getLocalDate(&day, &weekday);

    if (!lastWeek and dayOfWeek == weekday)
        return this->getStepsToday();
    else if(!lastWeek or (lastWeek and dayOfWeek == weekday))
        return this->_stepsCache[dayOfWeek];
    else
        return 0; // In that case we don't have any history left anymore - just reply with a zero...
}

uint32_t OswHal::Environment::getStepsAverage() {
    return this->getStepsTotalWeek() / 7; // getStepsTotalWeek() will call commitStepStatistics() anyways...
}
#endif

uint32_t OswHal::Environment::getStepsTotal() {
#ifdef OSW_FEATURE_STATS_STEPS
    return this->_stepsSum + this->getStepsToday();
#else
    return getStepsToday();
#endif
}

uint32_t OswHal::Environment::getStepsTotalWeek() {
#ifdef OSW_FEATURE_STATS_STEPS
    this->commitStepStatistics();
    uint32_t sum = 0;
    uint32_t currDoM = 0;  // Unused, but required by function signature
    uint32_t currDoW = 0;
    OswHal::getInstance()->getLocalDate(&currDoM, &currDoW);
    for (uint8_t i = 0; i < 7; i++) {
        if (i == currDoW) {
            sum = sum + this->getStepsToday();
        }
        sum = sum + this->_stepsCache[i];
    }
    return sum;
#else
    return this->getStepsTotal();
#endif
}
#endif

#if OSW_PLATFORM_ENVIRONMENT_PRESSURE == 1
float OswHal::Environment::getPressure() {
    if(!this->pressSensor)
        throw std::runtime_error("No pressure provider!");
    return this->pressSensor->getPressure();
}
#endif

#if OSW_PLATFORM_ENVIRONMENT_HUMIDITY == 1
float OswHal::Environment::getHumidity() {
    if(!this->humiSensor)
        throw std::runtime_error("No humidity provider!");
    return this->humiSensor->getHumidity();
}
#endif

#if OSW_PLATFORM_ENVIRONMENT_MAGNETOMETER == 1
int OswHal::Environment::getMagnetometerAzimuth() {
    if(!this->magSensor)
        throw std::runtime_error("No magnetometer provider!");
    return this->magSensor->getMagnetometerAzimuth();
}
#endif

#if OSW_SERVICE_NOTIFIER == 1
void OswHal::Environment::setupNotifications() {
    Preferences prefs;
    auto res = prefs.begin(PREFS_NOTIFS, false);
    assert(res);
    auto bytes = prefs.getBytes(PREFS_NOTIFS, &this->_notifs, sizeof(this->_notifs));
    if(bytes != sizeof(this->_notifs)) {
        for(size_t i = 0; i < _notifCount; i++)
            this->_notifs[i] = {};
        res = prefs.putBytes(PREFS_NOTIFS, &this->_notifs, sizeof(this->_notifs)) == sizeof(this->_notifs);
        assert(res);
    } else {
        res = bytes == sizeof(this->_notifs);
        assert(res);
    }
    prefs.end();
}

std::multimap<NotificationData::first_type, const NotificationData::second_type> OswHal::Environment::getNotifications()
{
    std::multimap<NotificationData::first_type, const NotificationData::second_type> notifications;
    for (size_t i = 0; i < _notifCount; ++i) {
        if (this->_notifs[i].message[0] && this->_notifs[i].publisher[0]) {
            std::array<bool, 7> daysOfWeek{};
            for (auto j = 0; j < 7; ++j) {
                daysOfWeek[j] = this->_notifs[i].daysOfWeek[j];
            }
            auto timeCreated = std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> {
                std::chrono::duration{std::chrono::seconds{this->_notifs[i].id}}
            };
            auto notification = Notification{timeCreated,
                                             this->_notifs[i].publisher,
                                             this->_notifs[i].message,
                                             daysOfWeek,
                                             this->_notifs[i].isPersistent};
            notifications.insert({std::chrono::time_point_cast<std::chrono::seconds>(
                                      std::chrono::system_clock::from_time_t(this->_notifs[i].timeToFire)),
                                  notification});
        }
    }
    return notifications;
}

void OswHal::Environment::commitNotifications(std::multimap<NotificationData::first_type, const NotificationData::second_type> notifications) {
    Preferences prefs;
    auto res = prefs.begin(PREFS_NOTIFS, false);
    assert(res);
    size_t i = 0;
    for (auto it = notifications.begin(); i < _notifCount && it != notifications.end(); ++it) {
        this->_notifs[i].timeToFire = std::chrono::system_clock::to_time_t(it->first);
        this->_notifs[i].id = it->second.getId();
        std::snprintf(this->_notifs[i].message, std::size(this->_notifs[i].message), "%s", it->second.getMessage().c_str());
        std::snprintf(this->_notifs[i].publisher, std::size(this->_notifs[i].publisher), "%s", it->second.getPublisher().c_str());
        this->_notifs[i].isPersistent = it->second.getPersistence();
        auto daysOfWeek = it->second.getDaysOfWeek();
        for (size_t j = 0; j < daysOfWeek.size(); ++j) {
            this->_notifs[i].daysOfWeek[j] = daysOfWeek[j];
        }
        ++i;
    }
    while (i < _notifCount) {
        this->_notifs[i] = {};
        ++i;
    }
    res = prefs.putBytes(PREFS_NOTIFS, &this->_notifs, sizeof(this->_notifs)) == sizeof(this->_notifs);
    assert(res);
    prefs.end();
}

#endif