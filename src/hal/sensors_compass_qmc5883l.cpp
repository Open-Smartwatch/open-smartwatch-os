#ifdef GPS_EDITION
#include <QMC5883LCompass.h>
#include <Wire.h>

#include "osw_hal.h"
#include "osw_pins.h"

QMC5883LCompass qmc5883l;

void OswHal::setupCompass(void) {
  qmc5883l.init();
  qmc5883l.setSmoothing(10, true);
  qmc5883l.setCalibration(-1708, 1190, -1841, 1135, -2370, 1636);
}

void OswHal::updateCompass(void) { qmc5883l.read(); }

void OswHal::setCompassCalibration(int x_min, int x_max, int y_min, int y_max, int z_min, int z_max) {
  qmc5883l.setCalibration(x_min, x_max, y_min, y_max, z_min, z_max);
}

void OswHal::stopCompass(void) {
  // TODO: set sleep mode via I2C write
}

int OswHal::getCompassX(void) { return qmc5883l.getX(); }
int OswHal::getCompassY(void) { return qmc5883l.getY(); }
int OswHal::getCompassZ(void) { return qmc5883l.getZ(); }

int OswHal::getCompassAzimuth(void) {
  // NOT (y,x),(y,z) (z,y)  (x,y)
  int a = atan2(qmc5883l.getY(), qmc5883l.getX()) * 180.0 / PI;
  return a < 0 ? 360 + a : a;
}

byte OswHal::getCompassBearing(void) {
  int a = getCompassAzimuth();
  return qmc5883l.getBearing(a);
}

#endif
