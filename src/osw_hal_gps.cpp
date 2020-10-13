#include <HardwareSerial.h>
#include <TinyGPS++.h>

#include "osw_hal.h"
#include "osw_pins.h"

HardwareSerial SerialGPS(1);
TinyGPSPlus gps;

void OswHal::setupGps(void) {
  pinMode(GPS_FON, OUTPUT);
  digitalWrite(GPS_FON, HIGH);
  SerialGPS.begin(9600, SERIAL_8N1, RX1, TX1);
}

void OswHal::gpsFullOnGps(void) {
  SerialGPS.print("$PMTK353,1,0,0,0,0*2A\r\n");  // Full On: GPS
}

void OswHal::gpsFullOnGpsGlonassBeidu(void) {
  SerialGPS.print("$PMTK353,1,1,1,0,0*2A\r\n");  // Full On: GPS, GLONASS, Galileo
}

void OswHal::redirectGpsToSerial(void) {
  while (SerialGPS.available() > 0) {
    Serial.write(SerialGPS.read());
  }
}

void OswHal::parseGps(void) {
  while (SerialGPS.available() > 0) {
    gps.encode(SerialGPS.read());
  }
}
