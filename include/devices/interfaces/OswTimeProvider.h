#pragma once

#include <time.h>
#include <list>

#include <devices/OswDevice.h>

class OswTimeProvider : public OswDevice {
public:
    virtual time_t getUTCTime() = 0;
    virtual void setUTCTime(const time_t& epoch) = 0;

    virtual unsigned char getTimeProviderPriority() = 0;
    static const std::list<OswTimeProvider*>* getAllTimeDevices() {
        return &allDevices;
    };
protected:
    OswTimeProvider() : OswDevice() {
        this->allDevices.push_back(this);
    };
    ~OswTimeProvider() {
        this->allDevices.remove(this);
    };
private:
    static std::list<OswTimeProvider*> allDevices;
};