#ifdef GPS_EDITION
#include <BME280I2C.h>
#include <Wire.h>

#include "osw_hal.h"
#include "osw_pins.h"

BME280I2C bme280;

BME280I2C::Settings settings(BME280::OSR_X1, BME280::OSR_X1, BME280::OSR_X1, BME280::Mode_Forced,
                             BME280::StandbyTime_1000ms, BME280::Filter_Off, BME280::SpiEnable_False,
                             BME280I2C::I2CAddr_0x76  // I2C address. I2C specific.
);

void OswHal::setupEnvironmentSensor(void) {
  if (bme280.begin()) {
    _hasBME280 = true;
    settings.mode = BME280::Mode_Forced;
    bme280.setSettings(settings);
  }
}

void OswHal::stopEnvironmentSensor(void) {
  if (_hasBME280) {
    _hasBME280 = false;
    settings.mode = BME280::Mode_Sleep;
    bme280.setSettings(settings);
  }
}

void OswHal::updateEnvironmentSensor(void) {
  if (_hasBME280) {
    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_Pa);

    bme280.read(_pres, _temp, _hum, tempUnit, presUnit);
  }
}

float OswHal::getPressure(void) { return _pres; }
float OswHal::getTemperature(void) { return _temp; }
float OswHal::getHumidtiy(void) { return _hum; }

#endif
