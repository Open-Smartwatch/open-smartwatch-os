#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include "BlueDot_BMA400.h"
#include "bma400_defs.h"
#include "osw_hal.h"
#include "osw_pins.h"

Adafruit_BME280 bme;
BlueDot_BMA400 bma400 = BlueDot_BMA400();
void IRAM_ATTR isrAlarm() { Serial.println("Alarm"); }

void OswHal::setupSensors() {
  Wire.begin(SDA, SCL, 100000L);
  uint8_t status = bme.begin(0x76, &Wire);
  if (!status) {
    _hasBME280 = false;
  }

  // See: https://platformio.org/lib/show/7125/BlueDot%20BMA400%20Library
  bma400.parameter.I2CAddress = 0x14;                   // default I2C address
  bma400.parameter.powerMode = 0x02;                    // normal mode
  bma400.parameter.measurementRange = BMA400_RANGE_2G;  // 2g range
  bma400.parameter.outputDataRate = BMA400_ODR_100HZ;   // 50 Hz
  bma400.parameter.oversamplingRate = 0x03;             // highest oversampling

  _hasBMA400 = bma400.init() == 0x90;

  bma400.enableStepCounter();

  // TODO: why is chip ID 0 ?
  // Serial.println(bma400.checkID(), 16);

  // if (_hasBMA400) {
  // } else {
  //   Serial.println("has no BMA400");
  // }

  // pinMode(BMA_INT_1, INPUT);
  // pinMode(BMA_INT_2, INPUT);

  // attachInterrupt(BMA_INT_2, isrStepDetect, RISING);
}

bool OswHal::hasBME280(void) { return _hasBME280; }
bool OswHal::hasBMA400(void) { return _hasBMA400; }

float OswHal::getPressureHPa(void) { return bme.readPressure() / 100.0F; }

void OswHal::updateAccelerometer(void) { bma400.readData(); }
float OswHal::getAccelerationX(void) { return bma400.parameter.acc_x; };
float OswHal::getAccelerationY(void) { return bma400.parameter.acc_y; };
float OswHal::getAccelerationZ(void) { return bma400.parameter.acc_z; };

uint32_t OswHal::getStepCount(void) { return bma400.readStepCount(); };
void OswHal::enableStepDetection(void) { bma400.enableStepCounter(); };
void OswHal::disableStepDetection(void) { bma400.disableStepCounter(); };
uint8_t OswHal::getActivityMode(void) { return bma400.readActivityMode(); };