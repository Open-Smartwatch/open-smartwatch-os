#include <stdexcept>

#include <BME280I2C.h>
#include <Wire.h>

#include <devices/bme280.h>
#include "osw_pins.h"

BME280I2C bme280;

BME280I2C::Settings settings(BME280::OSR_X1, BME280::OSR_X1, BME280::OSR_X1, BME280::Mode_Forced,
                             BME280::StandbyTime_1000ms, BME280::Filter_Off, BME280::SpiEnable_False,
                             BME280I2C::I2CAddr_0x76  // I2C address. I2C specific.
);

void OswDevices::BME280::setup() {
  if (bme280.begin()) {
    settings.mode = ::BME280::Mode_Forced;
    bme280.setSettings(settings);
  } else
    throw std::runtime_error("Failed to initialize BME280!");
}

void OswDevices::BME280::stop() {
  settings.mode = ::BME280::Mode_Sleep;
  bme280.setSettings(settings);
}

void OswDevices::BME280::update() {
  ::BME280::TempUnit tempUnit(::BME280::TempUnit_Celsius);
  ::BME280::PresUnit presUnit(::BME280::PresUnit_Pa);

  bme280.read(_pres, _temp, _hum, tempUnit, presUnit);
}

float OswDevices::BME280::getPressure() { return _pres; }
float OswDevices::BME280::getTemperature() { return _temp; }
float OswDevices::BME280::getHumidity() { return _hum; }