#pragma once

#include <list>

class OswDevice {
public:
    virtual void setup() = 0;
    virtual void update() = 0;
    virtual void reset() = 0;
    virtual void stop() = 0;

    static const std::list<OswDevice*>* getAllDevices() {
        return &allDevices;
    };
protected:
    OswDevice() {
        this->allDevices.push_back(this);
    };
    ~OswDevice() {
        this->allDevices.remove(this);
    };
private:
    static std::list<OswDevice*> allDevices;
};
