#pragma once

#include <list>

class OswDevice {
public:
    virtual void setup() = 0;
    virtual void update() = 0;
    virtual void reset() = 0;
    virtual void stop() = 0;

    virtual const char* getName() = 0;

    static const std::list<OswDevice*>* getAllDevices() {
        return &allDevices;
    };
protected:
    OswDevice();
    virtual ~OswDevice();
private:
    static std::list<OswDevice*> allDevices;
};
