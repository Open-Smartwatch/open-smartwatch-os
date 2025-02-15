#include OSW_TARGET_PLATFORM_HEADER
#if OSW_PLATFORM_HARDWARE_QMC5883P == 1
#include <stdexcept>

#include <osw_hal.h>
#include "osw_pins.h"

#include <devices/qmc5883p.h>
#define I2C_ADDRESS 0x2C

void OswDevices::QMC5883P::setup() {
    // define sign for XYZ axis
    wire->beginTransmission(I2C_ADDRESS);
    wire->write(0x29);
    wire->write(0x06);
    wire->endTransmission();
    // define Set/Reset mode with Set/Reset On, Field Range 8 Gauss)
    wire->beginTransmission(I2C_ADDRESS);
    wire->write(0x0B);
    wire->write(0x08);
    wire->endTransmission();
    // set normal mode, set ODR=200Hz
    wire->beginTransmission(I2C_ADDRESS);
    wire->write(0x0A);
    wire->write(0xCD);
    wire->endTransmission();
}

void OswDevices::QMC5883P::update() {
    // check ready register
    wire->beginTransmission(I2C_ADDRESS);
    wire->write(0x09);  // set read address to 0x09
    wire->endTransmission();
    wire->requestFrom(I2C_ADDRESS, 1);  // request 1 byte

    int regReady = wire->read() & 0x01;  // read 1 byte from buffer and only keep DRDY

    if (!regReady) {
    return;
    }

    // read data registers
    wire->beginTransmission(I2C_ADDRESS);
    wire->write(0x01);  // set read address to 0x01
    wire->endTransmission();
    wire->requestFrom(I2C_ADDRESS, 6);  // requst 6 bytes

    // Return XYZ readings, order is X_lsb, X_msb, Y_lsb, Y_msb, ...
    sensorData[0] = wire->read() | (wire->read() << 8);
    sensorData[1] = wire->read() | (wire->read() << 8);
    sensorData[2] = wire->read() | (wire->read() << 8);
}

void OswDevices::QMC5883P::stop() {
    // suspsend mode example
    wire->beginTransmission(I2C_ADDRESS);
    wire->write(0x0A);
    wire->write(0x00);
    wire->endTransmission();
}

void OswDevices::QMC5883P::setMagnetometerCalibration(int x_min, int x_max, int y_min, int y_max, int z_min, int z_max) {
    // TODO
}

int OswDevices::QMC5883P::getMagnetometerX() {
    return  sensorData[0];
}
int OswDevices::QMC5883P::getMagnetometerY() {
    return  sensorData[1] ;
}
int OswDevices::QMC5883P::getMagnetometerZ() {
    return sensorData[2];
}

int OswDevices::QMC5883P::getMagnetometerAzimuth() {
    // NOT (y,x),(y,z) (z,y)  (x,y)
    int a = atan2( sensorData[1], sensorData[0]) * 180.0f / PI;
    a = a < 0 ? 360 + a : a;

#if defined(GPS_EDITION_ROTATED)
    a = 360 - a;
#endif
    return a;
}

byte OswDevices::QMC5883P::getMagnetometerBearing() {
    int az = getMagnetometerAzimuth();
    unsigned long a = ( az > -0.5 ) ? az / 22.5 : (az+360)/22.5;
	unsigned long r = a - (int)a;
	byte sexdec = 0;	
	sexdec = ( r >= .5 ) ? ceil(a) : floor(a);
    return sexdec;
}
#endif
