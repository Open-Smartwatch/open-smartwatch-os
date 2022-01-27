#include <osw_hal.h>

void OswHal::Environment::setup() {
    #if OSW_PLATFORM_HARDWARE_BMA400 == 1
        this->setup_BMA400();
    #endif
    #if OSW_PLATFORM_HARDWARE_BME280 == 1
        this->setup_BME280();
    #endif
    #if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
        this->setupStepStatistics();
    #endif
    // The magnetometer is currently not setup/stopped by the hal. This should change.
}

void OswHal::Environment::stop() {
    #if OSW_PLATFORM_HARDWARE_BME280 == 1
        this->stop_BME280();
    #endif
    // The magnetometer is currently not setup/stopped by the hal. This should change.
}

void OswHal::Environment::update() {
    #if OSW_PLATFORM_HARDWARE_BMA400 == 1
        this->update_BMA400();
    #endif
    #if OSW_PLATFORM_HARDWARE_QMC5883L == 1
        this->update_QMC5883L();
    #endif
}

#if OSW_PLATFORM_ENVIRONMENT_TEMPERATURE == 1
float OswHal::Environment::getTemperature() {
    #if OSW_PLATFORM_HARDWARE_BME280 == 1
        return this->getTemperature_BME280();
    #elif OSW_PLATFORM_HARDWARE_BMA400 == 1
        return this->getTemperature_BMA400();
    #else
        // TODO Integrate the other sensors!
        #error "No temperature provider!"
    #endif
}
#endif

#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
float OswHal::Environment::getAccelerationX() {
    #if OSW_PLATFORM_HARDWARE_BMA400 == 1
        return this->getAccelerationX_BMA400();
    #else
        #error "No acceleration provider!"
    #endif
}

float OswHal::Environment::getAccelerationY() {
    #if OSW_PLATFORM_HARDWARE_BMA400 == 1
        return this->getAccelerationY_BMA400();
    #else
        #error "No acceleration provider!"
    #endif
}

float OswHal::Environment::getAccelerationZ() {
    #if OSW_PLATFORM_HARDWARE_BMA400 == 1
        return this->getAccelerationZ_BMA400();
    #else
        #error "No acceleration provider!"
    #endif
}
#endif

#if OSW_PLATFORM_ENVIRONMENT_PRESSURE == 1
float OswHal::Environment::getPressure() {
    #if OSW_PLATFORM_HARDWARE_BME280 == 1
        return this->getPressure_BME280();
    #else
        #error "No pressure provider!"
    #endif
}
#endif

#if OSW_PLATFORM_ENVIRONMENT_HUMIDITY == 1
float OswHal::Environment::getHumidtiy() {
    #if OSW_PLATFORM_HARDWARE_BME280 == 1
        return this->getHumidtiy_BME280();
    #else
        #error "No humidtiy provider!"
    #endif
}
#endif

#if OSW_PLATFORM_ENVIRONMENT_MAGNETOMETER == 1
int OswHal::Environment::getMagnetometerAzimuth() {
    #if OSW_PLATFORM_HARDWARE_QMC5883L == 1
        return this->getMagnetometerAzimuth_QMC5883L();
    #else
        #error "No magnetometer provider!"
    #endif
}
#endif