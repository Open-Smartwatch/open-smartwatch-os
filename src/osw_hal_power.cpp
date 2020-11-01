#include "osw_hal.h"
#include "osw_pins.h"

void OswHal::setupPower(void) {
  pinMode(STAT_PWR, INPUT);
  pinMode(B_MON, INPUT);
  pinMode(MODE_SYNC, OUTPUT);

  digitalWrite(MODE_SYNC, LOW);
}

boolean OswHal::isCharging(void) {
  return digitalRead(STAT_PWR);  // TODO: check if 1 = charging or not :)
}

float OswHal::getBatteryVoltage(void) {
  // 510k/100k -> 0.01mA quiescent current
  float r1 = 510000;  // 510k
  float r2 = 100000;  // 100k
  // ESP32: analogRead returns a value 0-4095
  float in = ((float)analogRead(B_MON)) / 4096.0;
  float vRef = 3.3;  // V
  return vRef * in * ((r1 + r2) / r2);
}