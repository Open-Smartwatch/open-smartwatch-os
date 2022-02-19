#pragma once

#include <list>

#include <devices/OswDevice.h>

class OswPressureProvider : public OswDevice {
public:
    virtual float getPressure() = 0;

    virtual unsigned char getPressureProviderPriority() = 0;
    static const std::list<OswPressureProvider*>* getAllPressureDevices() {
        return &allDevices;
    };
protected:
    OswPressureProvider() : OswDevice() {
        this->allDevices.push_back(this);
    };
    ~OswPressureProvider() {
        this->allDevices.remove(this);
    };
private:
    static std::list<OswPressureProvider*> allDevices;
};