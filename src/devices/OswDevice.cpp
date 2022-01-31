#include <devices/OswDevice.h>

std::list<OswDevice*> OswDevice::allDevices;

OswDevice::OswDevice() {
    this->allDevices.push_back(this);
};

OswDevice::~OswDevice() {
    this->allDevices.remove(this);
};