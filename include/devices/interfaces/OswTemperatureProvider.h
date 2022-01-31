#pragma once

#include <list>

#include <devices/OswDevice.h>

class OswTemperatureProvider : public OswDevice {
public:
    virtual float getTemperature() = 0;

    virtual unsigned char getTemperatureProviderPriority() = 0;
    static const std::list<OswTemperatureProvider*>* getAllTemperatureDevices() {
        return &allDevices;
    };
protected:
    OswTemperatureProvider() : OswDevice() {
        this->allDevices.push_back(this);
    };
    ~OswTemperatureProvider() {
        this->allDevices.remove(this);
    };
private:
    static std::list<OswTemperatureProvider*> allDevices;
};