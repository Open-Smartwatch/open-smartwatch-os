#include <HardwareSerial.h>
#include <NMEAGPS.h>
#define DEBUG_PORT Serial  // default for most sketches
#define gpsPort SerialGPS

#include "osw_hal.h"
#include "osw_pins.h"

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
HardwareSerial SerialGPS(1);
NMEAGPS nmeaGps;
static gps_fix fix;

NMEAGPS* OswHal::gps(void) { return &nmeaGps; }
void OswHal::setupGps(void) {
  pinMode(GPS_FON, OUTPUT);
  digitalWrite(GPS_FON, HIGH);
  SerialGPS.begin(9600, SERIAL_8N1, RX1, TX1);
}
bool OswHal::hasGPS(void) { return _hasGPS; }
bool OswHal::hasGPSFix(void) { return fix.latitude() != 0 || fix.longitude() != 0; }
gps_fix* OswHal::gpsFix(void) { return &fix; }
double OswHal::gpsLat(void) { return fix.latitude(); }
double OswHal::gpsLon(void) { return fix.longitude(); }

bool OswHal::isDebugGPS() { return _debugGPS; }
void OswHal::setDebugGPS(bool debugGPS) { _debugGPS = debugGPS; }

void OswHal::gpsForceOn(boolean on) { digitalWrite(GPS_FON, on ? HIGH : LOW); }

uint8_t OswHal::gpsNumSatellites(void) { return fix.satellites; }

// TODO: add more GPS commands here
void OswHal::gpsFullOnGps(void) {
  digitalWrite(GPS_FON, HIGH);
  delay(200);
  SerialGPS.print("$PMTK353,1,0,0,0,0*2A\r\n");  // Full On: GPS
}

void OswHal::gpsFullOnGpsGlonassBeidu(void) {
  digitalWrite(GPS_FON, HIGH);
  delay(200);
  SerialGPS.print("$PMTK353,1,1,1,0,0*2A\r\n");  // Full On: GPS, GLONASS, Galileo
}

void OswHal::gpsAdvancedPowerSave(void) {
  // AlwaysLocate on Quectel
  digitalWrite(GPS_FON, HIGH);
  delay(200);
  SerialGPS.print("$PMTK225,8*23\r\n");
}

void OswHal::gpsStandBy(void) {
  digitalWrite(GPS_FON, HIGH);
  delay(200);
  SerialGPS.print("$PMTK161,0*28\r\n");
}

void OswHal::gpsBackupMode(void) {
  SerialGPS.print("$PMTK225,4*2F\r\n");
  digitalWrite(GPS_FON, LOW);
}

// only use for manual debugging
void OswHal::gpsToSerial(void) {
  if (!_debugGPS) {
    while (SerialGPS.available() > 0) {
      _hasGPS = true;
      Serial.write(SerialGPS.read());
    }
  }
}

void OswHal::gpsParse(void) {
  if (!_debugGPS) {
    while (nmeaGps.available(gpsPort)) {
      _hasGPS = true;
      fix = nmeaGps.read();
    }
  }
}

HardwareSerial OswHal::getSerialGPS(void) { return SerialGPS; }

#endif
