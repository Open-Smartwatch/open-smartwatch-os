#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)

#include <QMC5883LCompass.h>
#include <Wire.h>

#include "osw_hal.h"
#include "osw_pins.h"

QMC5883LCompass qmc5883l;

void OswHal::setupMagnetometer(void) {
  qmc5883l.init();
  qmc5883l.setSmoothing(10, true);
}

void OswHal::updateMagnetometer(void) { qmc5883l.read(); }

void OswHal::setMagnetometerCalibration(int x_min, int x_max, int y_min, int y_max, int z_min, int z_max) {
  qmc5883l.setCalibration(x_min, x_max, y_min, y_max, z_min, z_max);
}

void OswHal::stopMagnetometer(void) {
  // TODO: set sleep mode via I2C write
}

int OswHal::getMagnetometerX(void) { return qmc5883l.getX(); }
int OswHal::getMagnetometerY(void) { return qmc5883l.getY(); }
int OswHal::getMagnetometerZ(void) { return qmc5883l.getZ(); }

int OswHal::getMagnetometerAzimuth(void) {
  // NOT (y,x),(y,z) (z,y)  (x,y)
  int a = atan2(qmc5883l.getY(), qmc5883l.getX()) * 180.0 / PI;
  a = a < 0 ? 360 + a : a;

#if defined(GPS_EDITION_ROTATED)
  a = 360 - a;
#endif
  return a;
}

byte OswHal::getMagnetometerBearing(void) {
  int a = getMagnetometerAzimuth();
  return qmc5883l.getBearing(a);
}

#endif
