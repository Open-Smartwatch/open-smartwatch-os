#include <stdexcept>

#include <Preferences.h>
#include <osw_hal.h>

#define PREFS_STEPS "PS"
#define PREFS_STEPS_DAYLASTCHECKED "L"
#define PREFS_STEPS_STATS "S"
#define PREFS_STEPS_ALL "A"

void OswHal::Environment::updateProviders() {
    // In case we come from deepsleep (or whenever the available devices change), we should first scan the current available devices for possible providers...
#if OSW_PLATFORM_ENVIRONMENT_TEMPERATURE == 1
    for(auto& d : *OswTemperatureProvider::getAllTemperatureDevices()) {
        if(this->tempSensor == nullptr or this->tempSensor->getTemperatureProviderPriority() < d->getTemperatureProviderPriority())
            this->tempSensor = d;
    }
#ifndef NDEBUG
    if(!this->tempSensor)
        Serial.println(String(__FILE__) + ": Temperature API enabled, but no provider available!");
#endif
#endif
#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
    for(auto& d : *OswAccelerationProvider::getAllAccelerationDevices()) {
        if(this->accelSensor == nullptr or this->accelSensor->getAccelerationProviderPriority() < d->getAccelerationProviderPriority())
            this->accelSensor = d;
    }
#ifndef NDEBUG
    if(!this->tempSensor)
        Serial.println(String(__FILE__) + ": Acceleration API enabled, but no provider available!");
#endif
#endif
#if OSW_PLATFORM_ENVIRONMENT_MAGNETOMETER == 1
    for(auto& d : *OswMagnetometerProvider::getAllMagnetometerDevices()) {
        if(this->magSensor == nullptr or this->magSensor->getMagnetometerProviderPriority() < d->getMagnetometerProviderPriority())
            this->magSensor = d;
    }
#ifndef NDEBUG
    if(!this->tempSensor)
        Serial.println(String(__FILE__) + ": Magnetometer API enabled, but no provider available!");
#endif
#endif
#if OSW_PLATFORM_ENVIRONMENT_PRESSURE == 1
    for(auto& d : *OswPressureProvider::getAllPressureDevices()) {
        if(this->pressSensor == nullptr or this->pressSensor->getPressureProviderPriority() < d->getPressureProviderPriority())
            this->pressSensor = d;
    }
#ifndef NDEBUG
    if(!this->tempSensor)
        Serial.println(String(__FILE__) + ": Pressure API enabled, but no provider available!");
#endif
#endif
#if OSW_PLATFORM_ENVIRONMENT_HUMIDITY == 1
    for(auto& d : *OswHumidityProvider::getAllHumidityDevices()) {
        if(this->humiSensor == nullptr or this->humiSensor->getHumidityProviderPriority() < d->getHumidityProviderPriority())
            this->humiSensor = d;
    }
#ifndef NDEBUG
    if(!this->tempSensor)
        Serial.println(String(__FILE__) + ": Humidity API enabled, but no provider available!");
#endif
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

void OswHal::Environment::setupStepStatistics() {
#ifdef OSW_FEATURE_STATS_STEPS
    if(!this->accelSensor)
        throw std::runtime_error("No acceleration provider!");
    Preferences prefs;
    bool res = prefs.begin(PREFS_STEPS, false); // Open in RW, just in case
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
    uint32_t lastDoW = prefs.getUInt(PREFS_STEPS_DAYLASTCHECKED);
    uint32_t currDoM = 0; // Unused, but required by function signature
    uint32_t currDoW = 0;
    OswHal::getInstance()->getDate(&currDoM, &currDoW);
    if(currDoW != lastDoW) {
        const uint32_t currentSteps = this->getStepsToday();
        this->_stepsCache[lastDoW] = currentSteps; // write current step to last dow
        this->_stepsSum += currentSteps; // Let's just hope this never rolls over...
#if OSW_PLATFORM_HARDWARE_BMA400 == 1
        if(OswHal::getInstance()->devices->bma400 == this->accelSensor)
            OswHal::getInstance()->devices->bma400->reset();
#else
#warning "Are you sure your acceleration provider does not need to be reset?"
#endif
        if(OswConfigAllKeys::stepsHistoryClear.get()) {
            if(currDoW > lastDoW) {
                // set stepscache to 0 in ]lastDoW, currDoW[
                for(uint32_t i = currDoW; lastDoW + 1 < i; i--)
                    this->_stepsCache[i - 1] = 0;
            } else {
                // set > last dow to 0 && set < curr dow to 0
                if(currDoW > 0)
                    for(uint32_t i = currDoW; 0 < i; i--)
                        this->_stepsCache[i - 1] = 0;
                for(uint32_t i = lastDoW + 1; i < 7; i++)
                    this->_stepsCache[i] = 0;
            }
        }

        // Check if today is the initialization day
        if (OswConfigAllKeys::resetDay.get() == currDoW + 1) // (e.g. 1 - 7 are days, 0 is disabled)
            this->_stepsSum = 0;

#ifndef NDEBUG
        Serial.println(String(__FILE__) + ": Updated steps from DoW " + String(lastDoW) + " to DoW " + String(currDoW));
#endif
        lastDoW = currDoW;
        // write step cache + stepsum
        res = prefs.putBytes(PREFS_STEPS_STATS, &this->_stepsCache, sizeof(this->_stepsCache)) == sizeof(this->_stepsCache);
        assert(res);
        res = prefs.putUInt(PREFS_STEPS_ALL, this->_stepsSum) == sizeof(this->_stepsSum);
        assert(res);
        res = prefs.putUInt(PREFS_STEPS_DAYLASTCHECKED, lastDoW) == sizeof(lastDoW);
        assert(res);
    }
    prefs.end();
#ifndef NDEBUG
    Serial.print(String(__FILE__) + ": Current step history (day " + String(currDoW) + ", today " + String(OswHal::getInstance()->devices->bma400->getStepCount()) + ", sum " + String(this->_stepsSum) + ") is: {");
    for(size_t i = 0; i < 7; i++) {
        if(i > 0)
            Serial.print(", ");
        if(i == currDoW)
            Serial.print("[");
        Serial.print(this->_stepsCache[i]);
        if(i == currDoW)
            Serial.print("]");
    }
    Serial.println("}");
#endif
#endif
}

uint32_t OswHal::Environment::getStepsToday() {
    if(!this->accelSensor)
        throw std::runtime_error("No acceleration provider!");
    return this->accelSensor->getStepCount();
}

#ifdef OSW_FEATURE_STATS_STEPS
uint32_t OswHal::Environment::getStepsOnDay(uint8_t dayOfWeek) {
    uint32_t day = 0;
    uint32_t weekday = 0;
    OswHal::getInstance()->getDate(&day, &weekday);
    if (dayOfWeek == weekday)
        return this->getStepsToday();
    return this->_stepsCache[dayOfWeek];
}
#endif

uint32_t OswHal::Environment::getStepsTotal() {
#ifdef OSW_FEATURE_STATS_STEPS
    return this->_stepsSum + this->getStepsToday();
#else
    return getStepsToday();
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