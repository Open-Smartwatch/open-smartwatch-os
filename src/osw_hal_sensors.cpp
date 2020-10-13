#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include "osw_hal.h"
#include "osw_pins.h"

Adafruit_BME280 bme;

void OswHal::setupSensors() {
  Wire.begin(SDA, SCL, 100000L);
  uint8_t status = bme.begin(0x76, &Wire);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
  }
}
