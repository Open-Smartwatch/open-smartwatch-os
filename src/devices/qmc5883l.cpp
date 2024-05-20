#ifndef OSW_EMULATOR
#include <stdexcept>

#include <osw_hal.h>
#include "osw_pins.h"

#include <devices/qmc5883l.h>

void OswDevices::QMC5883L::setup() {
    qmc5883l.init();
    qmc5883l.setSmoothing(10, true);

    // Error "detection"
    this->update();
    if (qmc5883l.getX() == 0 and qmc5883l.getY() == 0 and qmc5883l.getZ() == 0)
        throw std::runtime_error("Could not initialize QMC5883L");
}

void OswDevices::QMC5883L::update() {
    qmc5883l.read();
}

void OswDevices::QMC5883L::stop() {
    // TODO set sleep mode via I2C write
}

void OswDevices::QMC5883L::setMagnetometerCalibration(int x_min, int x_max, int y_min, int y_max, int z_min, int z_max) {
    qmc5883l.setCalibration(x_min, x_max, y_min, y_max, z_min, z_max);
}

int OswDevices::QMC5883L::getMagnetometerX() {
    return qmc5883l.getX();
}
int OswDevices::QMC5883L::getMagnetometerY() {
    return qmc5883l.getY();
}
int OswDevices::QMC5883L::getMagnetometerZ() {
    return qmc5883l.getZ();
}

int OswDevices::QMC5883L::getMagnetometerAzimuth() {
    // NOT (y,x),(y,z) (z,y)  (x,y)
    int a = atan2(qmc5883l.getY(), qmc5883l.getX()) * 180.0f / PI;
    a = a < 0 ? 360 + a : a;

#if defined(GPS_EDITION_ROTATED)
    a = 360 - a;
#endif
    return a;
}

byte OswDevices::QMC5883L::getMagnetometerBearing() {
    int a = getMagnetometerAzimuth();
    return qmc5883l.getBearing(a);
}
#endif