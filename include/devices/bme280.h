#pragma once

#include <devices/interfaces/OswTemperatureProvider.h>
#include <devices/interfaces/OswHumidityProvider.h>
#include <devices/interfaces/OswPressureProvider.h>

namespace OswDevices {
    class BME280 : public OswTemperatureProvider, public OswHumidityProvider, public OswPressureProvider {
    public:
        BME280() : OswTemperatureProvider(), OswHumidityProvider(), OswPressureProvider() {};
        virtual ~BME280() {};

        virtual void setup() override;
        virtual void update() override;
        virtual void reset() override {};
        virtual void stop() override;

        virtual inline const char* getName() override { return "BME280"; };

        virtual float getTemperature() override;
        virtual inline unsigned char getTemperatureProviderPriority() override { return 100; }; // This is a specialized device!

        virtual float getPressure() override;
        virtual inline unsigned char getPressureProviderPriority() override { return 100; }; // This is a specialized device!

        virtual float getHumidity() override;
        virtual inline unsigned char getHumidityProviderPriority() override { return 100; }; // This is a specialized device!
    private:
        float _temp = -100;
        float _hum = -100;
        float _pres = -100;
    };
};