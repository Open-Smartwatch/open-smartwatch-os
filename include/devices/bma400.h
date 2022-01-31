#pragma once

#include <devices/OswDevice.h>
#include <devices/interfaces/OswTemperatureProvider.h>
#include <devices/interfaces/OswAccelerationProvider.h>

namespace OswDevices {
    class BMA400 : public OswDevice, public OswTemperatureProvider, public OswAccelerationProvider {
    public:
        BMA400() : OswDevice(), OswTemperatureProvider(), OswAccelerationProvider() {};
        virtual ~BMA400() {};

        virtual void setup() override;
        virtual void update() override;
        virtual void reset() override;
        virtual void stop() override {};

        virtual float getTemperature() override;
        virtual inline unsigned char getTemperatureProviderPriority() override { return 20; }; // This sensor is not sooo good...

        virtual uint32_t getStepCount() override;
        virtual float getAccelerationX() override;
        virtual float getAccelerationY() override;
        virtual float getAccelerationZ() override;
        virtual inline unsigned char getAccelerationProviderPriority() override { return 100; }; // This is a specialized device!
        uint8_t getActivityMode();
    };
};