#pragma once

#include <list>

class OswPressureProvider {
public:
    virtual float getPressure() = 0;

    virtual unsigned char getPressureProviderPriority() = 0;
    static const std::list<OswPressureProvider*>* getAllPressureDevices() {
        return &allDevices;
    };
protected:
    OswPressureProvider() {
        this->allDevices.push_back(this);
    };
    ~OswPressureProvider() {
        this->allDevices.remove(this);
    };
private:
    static std::list<OswPressureProvider*> allDevices;
};