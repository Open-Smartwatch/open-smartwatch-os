#pragma once

#include <devices/interfaces/OswTemperatureProvider.h>
#include <devices/interfaces/OswAccelerationProvider.h>
#include <devices/interfaces/OswHumidityProvider.h>
#include <devices/interfaces/OswMagnetometerProvider.h>
#include <devices/interfaces/OswPressureProvider.h>

namespace OswDevices {
class Virtual : public OswTemperatureProvider, public OswAccelerationProvider, public OswHumidityProvider,
    public OswMagnetometerProvider, public OswPressureProvider {
  public:
    struct VirtualValues {
        float temperature = 42.0f;
        float pressure = 42.0f;
        float humidity = 42.0f;
        float accelerationX = 0.0f;
        float accelerationY = 0.0f;
        float accelerationZ = -9.81f;
        int magnetometerAzimuth = 0;
        uint32_t steps = 42;
    } values;

    Virtual(unsigned char priority = 0) : OswTemperatureProvider(), OswAccelerationProvider(), OswHumidityProvider(),
        OswMagnetometerProvider(), OswPressureProvider(), priority(priority) {};
    virtual ~Virtual() {};

    virtual void setup() override {};
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

    virtual inline uint32_t getStepCount() override {
        return this->values.steps;
    };
    virtual void resetStepCount() override {
        this->values.steps = 0;
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
    virtual inline unsigned char getAccelerationProviderPriority() override {
        return this->priority;
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
    virtual inline unsigned char getMagnetometerProviderPriority() override {
        return this->priority;
    };
  private:
    const unsigned char priority;
};
};