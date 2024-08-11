#pragma once
#include OSW_TARGET_PLATFORM_HEADER
#if OSW_PLATFORM_HARDWARE_BMI270 == 1

#include <devices/interfaces/OswTemperatureProvider.h>
#include <devices/interfaces/OswAccelerationProvider.h>
#include <bmi2_defs.h>
#include <bmi270.h>

namespace OswDevices {
class BMI270 : public OswAccelerationProvider, public OswTemperatureProvider {
  public:
    BMI270() : OswAccelerationProvider(), OswTemperatureProvider() {};
    virtual ~BMI270() {};

    virtual void setup() override;
    virtual void update() override;
    virtual void reset() override {};
    virtual void stop() override {};

    virtual inline const char* getName() override {
        return "BMI270";
    };
    virtual float getAccelerationX() override;
    virtual float getAccelerationY() override;
    virtual float getAccelerationZ() override;
    virtual uint32_t getStepCount() override;
    virtual void resetStepCount() override;
    virtual OswAccelerationProvider::ActivityMode getActivityMode() override;
    virtual inline unsigned char getAccelerationProviderPriority() override {
        return 100;
    }; // This is a specialized device!

    virtual float getTemperature() override;
    virtual inline unsigned char getTemperatureProviderPriority() override {
        return 20;
    }; // This sensor is not sooo good...
  private:
    bmi2_dev bmi2 = {};
    float accX = 0;
    float accY = 0;
    float accZ = 0;
    uint32_t step_count = 0;
    float temperature = 0;
    OswAccelerationProvider::ActivityMode activityMode = OswAccelerationProvider::ActivityMode::UNKNOWN;

    void updateAcceleration();
    void updateSteps();
    void updateTemperature();
    void updateActivityMode();
};
};
#endif