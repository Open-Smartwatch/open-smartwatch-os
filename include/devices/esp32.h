#pragma once

#include <devices/interfaces/OswTemperatureProvider.h>
#include <devices/interfaces/OswTimeProvider.h>

namespace OswDevices {
class NativeESP32 : public OswTemperatureProvider, public OswTimeProvider {
  public:
    NativeESP32() : OswTemperatureProvider(), OswTimeProvider() {};
    virtual ~NativeESP32() {};

    virtual void setup() override;
    virtual void update() override;
    virtual void reset() override {};
    virtual void stop() override {};

    virtual inline const char* getName() override {
        return "ESP32";
    };

    bool isTemperatureSensorAvailable();
    virtual float getTemperature() override;
    virtual inline unsigned char getTemperatureProviderPriority() override {
        return 20;
    }; // This sensor is not sooo good...

    void setClockResyncEnabled(const bool& enable);
    bool isClockResyncEnabled();
    void triggerNTPUpdate();
    bool checkNTPUpdate();
    virtual time_t getUTCTime() override;
    virtual void setUTCTime(const time_t& epoch) override;
    virtual inline unsigned char getTimeProviderPriority() override {
        return 40;
    }; // This is a specialized (bad) device!
    virtual time_t getTimezoneOffset(const time_t& timestamp, const String& timezone) override;
  private:
    const time_t successfulNTPTime = 1600000000; // This is the UNIX timestamp for the "Sunday, 13 September 2020 12:26:40" -> if the time of the ESP32 is at least this value, we consider the NTP update to be successful
    bool tempSensorIsBuiltIn = true;
    bool enableTimeResync = true;
    bool waitingForNTP = false;
};
};
