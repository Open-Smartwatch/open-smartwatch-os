#pragma once
#include OSW_TARGET_PLATFORM_HEADER
#if OSW_PLATFORM_HARDWARE_BMA400 == 1

#include <devices/interfaces/OswTemperatureProvider.h>
#include <devices/interfaces/OswAccelerationProvider.h>
#include <bma400_defs.h>
#include <bma400.h>

namespace OswDevices {
class BMA400 : public OswTemperatureProvider, public OswAccelerationProvider {
  public:
    BMA400() : OswTemperatureProvider(), OswAccelerationProvider() {};
    virtual ~BMA400() {};

    virtual void setup() override;
    virtual void update() override;
    virtual void reset() override {};
    virtual void stop() override {};

    virtual inline const char* getName() override {
        return "BMA400";
    };

    virtual float getTemperature() override;
    virtual inline unsigned char getTemperatureProviderPriority() override {
        return 20;
    }; // This sensor is not sooo good...

    virtual float getAccelerationX() override;
    virtual float getAccelerationY() override;
    virtual float getAccelerationZ() override;
    virtual uint32_t getStepCount() override;
    virtual void resetStepCount() override;
    virtual OswAccelerationProvider::ActivityMode getActivityMode() override;
    virtual inline unsigned char getAccelerationProviderPriority() override {
        return 100;
    }; // This is a specialized device!

  private:
    bma400_dev bma = {};
    float accelT = 0.0f;
    float accelX = 0.0f;
    float accelY = 0.0f;
    float accelZ = 0.0f;
    uint32_t step_count = 0;
    OswAccelerationProvider::ActivityMode activityMode = OswAccelerationProvider::ActivityMode::UNKNOWN;

    void setupTiltToWake();
};
};
#endif
