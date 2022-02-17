#pragma once

#include <devices/interfaces/OswTemperatureProvider.h>
#include <devices/interfaces/OswTimeProvider.h>

namespace OswDevices {
    class NativeESP32 : public OswTemperatureProvider, public OswTimeProvider {
    public:
        NativeESP32() : OswTemperatureProvider(), OswTimeProvider() {};
        virtual ~NativeESP32() {};

        virtual void setup() override;
        virtual void update() override {};
        virtual void reset() override {};
        virtual void stop() override {};

        virtual inline const char* getName() override { return "ESP32"; };

        bool isTemperatureSensorAvailable();
        virtual float getTemperature() override;
        virtual inline unsigned char getTemperatureProviderPriority() override { return 20; }; // This sensor is not sooo good...

        void triggerNTPUpdate();
        virtual time_t getUTCTime() override;
        virtual void setUTCTime(const time_t& epoch) override;
        virtual inline unsigned char getTimeProviderPriority() override { return 40; }; // This is a specialized (bad) device!
    private:
        bool tempSensorIsBuiltIn = true;
    };
};