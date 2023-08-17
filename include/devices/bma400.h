#pragma once
#if OSW_PLATFORM_HARDWARE_BMA400 == 1

#include <devices/interfaces/OswTemperatureProvider.h>
#include <devices/interfaces/OswAccelerationProvider.h>

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

    virtual uint32_t getStepCount() override;
    virtual void resetStepCount() override;
    virtual float getAccelerationX() override;
    virtual float getAccelerationY() override;
    virtual float getAccelerationZ() override;
    virtual inline unsigned char getAccelerationProviderPriority() override {
        return 100;
    }; // This is a specialized device!
    uint8_t getActivityMode();
};
};
#endif
