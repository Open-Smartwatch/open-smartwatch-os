#pragma once

#include <list>

class OswTemperatureProvider {
public:
    virtual float getTemperature() = 0;

    virtual unsigned char getTemperatureProviderPriority() = 0;
    static const std::list<OswTemperatureProvider*>* getAllTemperatureDevices() {
        return &allDevices;
    };
protected:
    OswTemperatureProvider() {
        this->allDevices.push_back(this);
    };
    ~OswTemperatureProvider() {
        this->allDevices.remove(this);
    };
private:
    static std::list<OswTemperatureProvider*> allDevices;
};