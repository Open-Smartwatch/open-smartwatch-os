#include <QMC5883LCompass.h>
#include <Wire.h>

#include <osw_hal.h>
#include "osw_pins.h"

#if OSW_PLATFORM_HARDWARE_QMC5883L == 1
QMC5883LCompass qmc5883l;

void OswHal::Environment::setup_QMC5883L() {
  qmc5883l.init();
  qmc5883l.setSmoothing(10, true);
  // It seems like this can never go wrong?! Do not ask.
}

void OswHal::Environment::update_QMC5883L() {
  qmc5883l.read();
}

void OswHal::Environment::setMagnetometerCalibration_QMC5883L(int x_min, int x_max, int y_min, int y_max, int z_min, int z_max) {
  qmc5883l.setCalibration(x_min, x_max, y_min, y_max, z_min, z_max);
}

void OswHal::Environment::stop_QMC5883L() {
  // TODO set sleep mode via I2C write
}

int OswHal::Environment::getMagnetometerX_QMC5883L() { return qmc5883l.getX(); }
int OswHal::Environment::getMagnetometerY_QMC5883L() { return qmc5883l.getY(); }
int OswHal::Environment::getMagnetometerZ_QMC5883L() { return qmc5883l.getZ(); }

int OswHal::Environment::getMagnetometerAzimuth_QMC5883L() {
  // NOT (y,x),(y,z) (z,y)  (x,y)
  int a = atan2(qmc5883l.getY(), qmc5883l.getX()) * 180.0 / PI;
  a = a < 0 ? 360 + a : a;

#if defined(GPS_EDITION_ROTATED)
  a = 360 - a;
#endif
  return a;
}

byte OswHal::Environment::getMagnetometerBearing_QMC5883L() {
  int a = getMagnetometerAzimuth();
  return qmc5883l.getBearing(a);
}
#endif