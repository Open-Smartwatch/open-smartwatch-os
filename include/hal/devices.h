#pragma once

#include <osw_util.h>
#include <osw_hal.h>
#include <devices/OswDevice.h>
#include <devices/bma400.h>
#include <devices/qmc5883l.h>
#include <devices/bme280.h>
#include <devices/ds3231mz.h>
#include <devices/esp32.h>
#include <devices/virtual.h>

class OswHal::Devices {
public:
    #if OSW_PLATFORM_HARDWARE_BMA400 == 1
    OswDevices::BMA400* bma400;
    #endif
    #if OSW_PLATFORM_HARDWARE_QMC5883L == 1
    OswDevices::QMC5883L* qmc5883l;
    #endif
    #if OSW_PLATFORM_HARDWARE_BME280 == 1
    OswDevices::BME280* bme280;
    #endif
    #if OSW_PLATFORM_HARDWARE_DS3231MZ == 1
    OswDevices::DS3231MZ* ds3231mz;
    #endif
    #if OSW_PLATFORM_HARDWARE_VIRTUAL == 1
    OswDevices::Virtual* virtualDevice;
    #endif
    OswDevices::NativeESP32* esp32;

    void setup(const bool& fromLightSleep);
    void update(); // Request devices to update their (cached) states
    void stop(const bool& toLightSleep);
protected:
    Devices();
    ~Devices();
    friend OswHal;
private:
};