#pragma once

#include "osw_hal.h"

#if OSW_PLATFORM_ENVIRONMENT == 1
class OswHal::Environment {
public:
    void setup(); // Init all environment sensors
    void reset(); // Reset all data on environment sensors
    void update(); // Updates all cached temperatures, pressure (...) values
    void stop(); // Shutdown all environment sensors

    #if OSW_PLATFORM_HARDWARE_BMA400 == 1
        void setup_BMA400();
        void reset_BMA400();
        void update_BMA400();
    #endif
    #if OSW_PLATFORM_HARDWARE_BME280 == 1
        void setup_BME280();
        void update_BME280();
        void stop_BME280();
    #endif
    #if OSW_PLATFORM_HARDWARE_QMC5883L == 1
        void setup_QMC5883L();
        void update_QMC5883L();
        void stop_QMC5883L();
    #endif

    #if OSW_PLATFORM_ENVIRONMENT_TEMPERATURE == 1
        float getTemperature(); // Get the temperature either by the sensor of the GPS edition (BME280) or from the RTC module
        #if OSW_PLATFORM_HARDWARE_DS3231MZ == 1
            float getTemperature_DS3231MZ();
        #endif
        #if OSW_PLATFORM_HARDWARE_BMA400 == 1
            float getTemperature_BMA400();
        #endif
        #if OSW_PLATFORM_HARDWARE_BME280 == 1
            float getTemperature_BME280();
        #endif
    #endif

    #if OSW_PLATFORM_ENVIRONMENT_PRESSURE == 1
        float getPressure();
        #if OSW_PLATFORM_HARDWARE_BME280 == 1
            float getPressure_BME280();
        #endif
    #endif

    #if OSW_PLATFORM_ENVIRONMENT_HUMIDITY == 1
        float getHumidtiy();
        #if OSW_PLATFORM_HARDWARE_BME280 == 1
            float getHumidtiy_BME280();
        #endif
    #endif

    #if OSW_PLATFORM_ENVIRONMENT_MAGNETOMETER == 1
        int getMagnetometerAzimuth();
        #if OSW_PLATFORM_HARDWARE_QMC5883L == 1
            int getMagnetometerX_QMC5883L();
            int getMagnetometerY_QMC5883L();
            int getMagnetometerZ_QMC5883L();
            int getMagnetometerAzimuth_QMC5883L();
            byte getMagnetometerBearing_QMC5883L();
            void setMagnetometerCalibration_QMC5883L(int x_min, int x_max, int y_min, int y_max, int z_min, int z_max);
        #endif
    #endif

    #if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
        float getAccelerationX();
        float getAccelerationY();
        float getAccelerationZ();
        #if OSW_PLATFORM_HARDWARE_BMA400 == 1
            uint32_t getStepCount_BMA400();
            uint8_t getActivityMode_BMA400();
            float getAccelerationX_BMA400();
            float getAccelerationY_BMA400();
            float getAccelerationZ_BMA400();
        #endif
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
    #if OSW_PLATFORM_HARDWARE_BME280 == 1
        float _temp = -100;
        float _hum = -100;
        float _pres = -100;
    #endif
};
#endif