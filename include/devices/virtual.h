#pragma once

#include <OswLogger.h>
#include <devices/interfaces/OswTemperatureProvider.h>
#include <devices/interfaces/OswAccelerationProvider.h>
#include <devices/interfaces/OswHumidityProvider.h>
#include <devices/interfaces/OswMagnetometerProvider.h>
#include <devices/interfaces/OswPressureProvider.h>
#include <devices/interfaces/OswTimeProvider.h>

namespace OswDevices {
class Virtual : public OswTemperatureProvider, public OswAccelerationProvider, public OswHumidityProvider,
    public OswMagnetometerProvider, public OswPressureProvider, public OswTimeProvider {
  public:
    struct VirtualValues {
        float temperature = 42.0f;
        float pressure = 42.0f;
        float humidity = 42.0f;
        float accelerationX = 0.0f;
        float accelerationY = 0.0f;
        float accelerationZ = -9.81f;
        OswAccelerationProvider::ActivityMode activityMode = OswAccelerationProvider::ActivityMode::UNKNOWN;
        int magnetometerAzimuth = 0;
        int magnetometerX = 0;
        int magnetometerY = 0;
        int magnetometerZ = 0;
        uint32_t steps = 42;
    } values;

    Virtual(unsigned char priority = 0) : OswTemperatureProvider(), OswAccelerationProvider(), OswHumidityProvider(),
        OswMagnetometerProvider(), OswPressureProvider(), OswTimeProvider(), priority(priority) {};
    virtual ~Virtual() {};

    virtual void setup() override {
        setenv("TZ", "UTC0", 1); // Force systems clock to correspond to UTC (this is especially important on POSIX systems)
    };
    virtual void update() override {};
    virtual void reset() override {};
    virtual void stop() override {};

    virtual inline const char* getName() override {
        return "Virtual";
    };

    virtual inline float getTemperature() override {
        return this->values.temperature;
    };
    virtual inline unsigned char getTemperatureProviderPriority() override {
        return this->priority;
    };

    virtual inline float getAccelerationX() override {
        return this->values.accelerationX;
    };
    virtual inline float getAccelerationY() override {
        return this->values.accelerationY;
    };
    virtual inline float getAccelerationZ() override {
        return this->values.accelerationZ;
    };
    virtual inline uint32_t getStepCount() override {
        return this->values.steps;
    };
    virtual void resetStepCount() override {
        this->values.steps = 0;
    };
    virtual inline unsigned char getAccelerationProviderPriority() override {
        return this->priority;
    };
    virtual OswAccelerationProvider::ActivityMode getActivityMode() override {
        return this->values.activityMode;
    };

    virtual inline float getHumidity() override {
        return this->values.humidity;
    };
    virtual inline unsigned char getHumidityProviderPriority() override {
        return this->priority;
    };

    virtual inline float getPressure() override {
        return this->values.pressure;
    };
    virtual inline unsigned char getPressureProviderPriority() override {
        return this->priority;
    };

    virtual inline int getMagnetometerAzimuth() override {
        return this->values.magnetometerAzimuth;
    };
    virtual int getMagnetometerX() override {
        return this->values.magnetometerX;
    };
    virtual int getMagnetometerY() override {
        return this->values.magnetometerY;
    };
    virtual int getMagnetometerZ() override {
        return this->values.magnetometerZ;
    };
    virtual inline unsigned char getMagnetometerProviderPriority() override {
        return this->priority;
    };

    virtual time_t getUTCTime() override {
        return time(nullptr);
    };
    virtual void setUTCTime(const time_t& epoch) {
        OSW_LOG_W("Virtual device does not support setting the time");
    };
    virtual unsigned char getTimeProviderPriority() override {
        return this->priority;
    };
#ifdef OSW_EMULATOR
    virtual time_t getTimezoneOffset(const time_t& timestamp, const String& timezone) override;
#endif
  private:
    const unsigned char priority;
};
};
