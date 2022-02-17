#pragma once

#include <Wire.h>
#include <RtcDS3231.h>

#include <devices/interfaces/OswTemperatureProvider.h>
#include <devices/interfaces/OswTimeProvider.h>

namespace OswDevices {
    class DS3231MZ : public OswTemperatureProvider, public OswTimeProvider {
    public:
        DS3231MZ() : OswTemperatureProvider(), OswTimeProvider(), Rtc(Wire) {};
        virtual ~DS3231MZ() {};

        virtual void setup() override;
        virtual void update() override;
        virtual void reset() override {};
        virtual void stop() override {};

        virtual inline const char* getName() override { return "DS3231MZ"; };

        virtual float getTemperature() override;
        virtual inline unsigned char getTemperatureProviderPriority() override { return 20; }; // This sensor is not sooo good...

        virtual time_t getUTCTime() override;
        virtual void setUTCTime(const time_t& epoch) override;
        virtual inline unsigned char getTimeProviderPriority() override { return 100; }; // This is a specialized device!
    private:
        RtcDS3231<TwoWire> Rtc;
        uint32_t _utcTime = 0;
    };
};