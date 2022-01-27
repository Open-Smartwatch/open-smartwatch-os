#include <stdexcept>

#include <BME280I2C.h>
#include <Wire.h>

#include "osw_hal.h"
#include "osw_pins.h"

#if OSW_PLATFORM_HARDWARE_BME280 == 1
BME280I2C bme280;

BME280I2C::Settings settings(BME280::OSR_X1, BME280::OSR_X1, BME280::OSR_X1, BME280::Mode_Forced,
                             BME280::StandbyTime_1000ms, BME280::Filter_Off, BME280::SpiEnable_False,
                             BME280I2C::I2CAddr_0x76  // I2C address. I2C specific.
);

void OswHal::Environment::setup_BME280() {
  if (bme280.begin()) {
    settings.mode = BME280::Mode_Forced;
    bme280.setSettings(settings);
  } else
    throw std::runtime_error("Failed to initialize BME280!");
}

void OswHal::Environment::stop_BME280() {
  settings.mode = BME280::Mode_Sleep;
  bme280.setSettings(settings);
}

void OswHal::Environment::update_BME280() {
  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_Pa);

  bme280.read(_pres, _temp, _hum, tempUnit, presUnit);
}

float OswHal::Environment::getPressure_BME280() { return _pres; }
float OswHal::Environment::getTemperature_BME280() { return _temp; }
float OswHal::Environment::getHumidtiy_BME280() { return _hum; }
#endif