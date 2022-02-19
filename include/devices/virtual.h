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
        Virtual(unsigned char priority = 0) : OswTemperatureProvider(), OswAccelerationProvider(), OswHumidityProvider(),
            OswMagnetometerProvider(), OswPressureProvider(), priority(priority) {};
        virtual ~Virtual() {};

        virtual void setup() override {};
        virtual void update() override {};
        virtual void reset() override {};
        virtual void stop() override {};

        virtual inline const char* getName() override { return "Virtual"; };

        virtual inline float getTemperature() override { return 42.0; };
        virtual inline unsigned char getTemperatureProviderPriority() override { return this->priority; };

        virtual inline uint32_t getStepCount() override { return 42; };
        virtual inline float getAccelerationX() override { return 0; };
        virtual inline float getAccelerationY() override { return 0; };
        virtual inline float getAccelerationZ() override { return 0; };
        virtual inline unsigned char getAccelerationProviderPriority() override { return this->priority; };

        virtual inline float getHumidity() override { return 42; };
        virtual inline unsigned char getHumidityProviderPriority() override { return this->priority; };

        virtual inline float getPressure() override { return 42; };
        virtual inline unsigned char getPressureProviderPriority() override { return this->priority; };

        virtual inline int getMagnetometerAzimuth() override { return 42; };
        virtual inline unsigned char getMagnetometerProviderPriority() override { return this->priority; };
    private:
        const unsigned char priority;
    };
};