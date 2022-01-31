#pragma once

#include <list>

class OswHumidityProvider {
public:
    virtual float getHumidity() = 0;

    virtual unsigned char getHumidityProviderPriority() = 0;
    static const std::list<OswHumidityProvider*>* getAllHumidityDevices() {
        return &allDevices;
    };
protected:
    OswHumidityProvider() {
        this->allDevices.push_back(this);
    };
    ~OswHumidityProvider() {
        this->allDevices.remove(this);
    };
private:
    static std::list<OswHumidityProvider*> allDevices;
};