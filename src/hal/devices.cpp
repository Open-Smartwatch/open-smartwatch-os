#include OSW_TARGET_PLATFORM_HEADER
#include <hal/devices.h>
#include <devices/OswDevice.h>

OswHal::Devices::Devices() {
    // We are initializing these not statically, as this WILL interfere with other static initializors!
#if OSW_PLATFORM_HARDWARE_BMA400 == 1
    this->bma400 = new OswDevices::BMA400();
#endif
#if OSW_PLATFORM_HARDWARE_BMI270 == 1
    this->bmi270 = new OswDevices::BMI270();
#endif
#if OSW_PLATFORM_HARDWARE_BMP581 == 1
    this->bmp581 = new OswDevices::BMP581();
#endif
#if OSW_PLATFORM_HARDWARE_QMC5883L == 1
    this->qmc5883l = new OswDevices::QMC5883L();
#endif
#if OSW_PLATFORM_HARDWARE_BME280 == 1
    this->bme280 = new OswDevices::BME280();
#endif
#if OSW_PLATFORM_HARDWARE_DS3231MZ == 1
    this->ds3231mz = new OswDevices::DS3231MZ();
#endif
#if OSW_PLATFORM_HARDWARE_VIRTUAL == 1
    this->virtualDevice = new OswDevices::Virtual(100);
#endif
#if OSW_PLATFORM_HARDWARE_ESP32 == 1
    this->esp32 = new OswDevices::NativeESP32();
#endif
}

OswHal::Devices::~Devices() {
#if OSW_PLATFORM_HARDWARE_ESP32 == 1
    delete this->esp32;
#endif
#if OSW_PLATFORM_HARDWARE_VIRTUAL == 1
    delete this->virtualDevice;
#endif
#if OSW_PLATFORM_HARDWARE_DS3231MZ == 1
    delete this->ds3231mz;
#endif
#if OSW_PLATFORM_HARDWARE_BME280 == 1
    delete this->bme280;
#endif
#if OSW_PLATFORM_HARDWARE_QMC5883L == 1
    delete this->qmc5883l;
#endif
#if OSW_PLATFORM_HARDWARE_BMI270 == 1
    delete this->bmi270;
#endif
#if OSW_PLATFORM_HARDWARE_BMA400 == 1
    delete this->bma400;
#endif
}

void OswHal::Devices::setup(const bool& fromLightSleep) {
    for(auto& d : *OswDevice::getAllDevices())
        d->setup();
}

void OswHal::Devices::update() {
    for(auto& d : *OswDevice::getAllDevices())
        d->update();
}

void OswHal::Devices::stop(const bool& toLightSleep) {
    for(auto& d : *OswDevice::getAllDevices())
        d->stop();
}
