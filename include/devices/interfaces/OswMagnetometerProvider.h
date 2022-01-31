#pragma once

#include <list>

#include <devices/OswDevice.h>

class OswMagnetometerProvider : public OswDevice {
public:
    virtual int getMagnetometerAzimuth() = 0;

    virtual unsigned char getMagnetometerProviderPriority() = 0;
    static const std::list<OswMagnetometerProvider*>* getAllMagnetometerDevices() {
        return &allDevices;
    };
protected:
    OswMagnetometerProvider() : OswDevice() {
        this->allDevices.push_back(this);
    };
    ~OswMagnetometerProvider() {
        this->allDevices.remove(this);
    };
private:
    static std::list<OswMagnetometerProvider*> allDevices;
};