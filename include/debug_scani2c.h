#ifndef DEBUG_SCAN_I2C_H
#define DEBUG_SCAN_I2C_H

#include <OswLogger.h>
#include <Arduino.h>

// Source: https://playground.arduino.cc/Main/I2cScanner/

void scanI2C() {
    byte error, address;
    int nDevices;

    OSW_LOG_I("Scanning...");

    nDevices = 0;
    for (address = 1; address < 127; address++) {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            OSW_LOG_I("I2C device found at address 0x", address < 16 ? '0' : '', msg, String(address, HEX), " !");

            nDevices++;
        } else if (error == 4) {
            OSW_LOG_E("Unknown error at address 0x", address < 16 ? "0" : "", String(address, HEX));
        }
    }
    if (nDevices == 0)
        OSW_LOG_I("No I2C devices found");
    else
        OSW_LOG_I("Done");
}

#endif
