#pragma once

#include <osw_hal.h>
#include <devices/interfaces/OswTemperatureProvider.h>
#include <devices/interfaces/OswAccelerationProvider.h>
#include <devices/interfaces/OswMagnetometerProvider.h>
#include <devices/interfaces/OswHumidityProvider.h>
#include <devices/interfaces/OswPressureProvider.h>

#if OSW_PLATFORM_ENVIRONMENT == 1
class OswHal::Environment {
public:
    void updateProviders(); // Retreive all environment sensors
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
        void setupStepStatistics();
        uint32_t getStepsToday();
        uint32_t getStepsTotal();
        #ifdef OSW_FEATURE_STATS_STEPS
            uint32_t getStepsOnDay(uint8_t dayOfWeek);
        #endif
    #endif

protected:
    Environment() {}
    ~Environment() {}
    friend OswHal;
private:
    #ifdef OSW_FEATURE_STATS_STEPS
        uint32_t _stepsCache[7] = {0};
        uint32_t _stepsSum = 0;
    #endif
    OswTemperatureProvider* tempSensor = nullptr;
    OswAccelerationProvider* accelSensor = nullptr;
    OswMagnetometerProvider* magSensor = nullptr;
    OswHumidityProvider* humiSensor = nullptr;
    OswPressureProvider* pressSensor = nullptr;
};
#endif