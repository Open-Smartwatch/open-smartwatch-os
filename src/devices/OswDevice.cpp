#include <devices/OswDevice.h>

std::set<OswDevice*> OswDevice::allDevices;

OswDevice::OswDevice() {
    this->allDevices.insert(this);
};

OswDevice::~OswDevice() {
    this->allDevices.erase(this);
};