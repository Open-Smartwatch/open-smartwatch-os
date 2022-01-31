#pragma once

#include <list>

class OswAccelerationProvider {
public:
    virtual uint32_t getStepCount() = 0;
    virtual float getAccelerationX() = 0;
    virtual float getAccelerationY() = 0;
    virtual float getAccelerationZ() = 0;

    virtual unsigned char getAccelerationProviderPriority() = 0;
    static const std::list<OswAccelerationProvider*>* getAllAccelerationDevices() {
        return &allDevices;
    };
protected:
    OswAccelerationProvider() {
        this->allDevices.push_back(this);
    };
    ~OswAccelerationProvider() {
        this->allDevices.remove(this);
    };
private:
    static std::list<OswAccelerationProvider*> allDevices;
};