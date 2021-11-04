
#include "./apps/tools/print_debug.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_pins.h>

#define SERIAL_BUF_SIZE 14

uint8_t y = 32;
uint8_t x = 52;
void printStatus(const char* setting, const char* value) {
  OswHal* hal = OswHal::getInstance();
  hal->gfx()->setTextCursor(x, y);
  hal->gfx()->print(setting);
  hal->gfx()->print(": ");
  hal->gfx()->print(value);
  y += 8;
}
void OswAppPrintDebug::setup() {
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
  OswHal::getInstance()->gpsFullOnGpsGlonassBeidu();
  OswHal::getInstance()->setupMagnetometer();
#endif
}
void OswAppPrintDebug::loop() {
  static String serialBuffer[SERIAL_BUF_SIZE];
  OswHal* hal = OswHal::getInstance();

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
  if (hal->btnHasGoneDown(BUTTON_2)) {
    hal->setDebugGPS(!hal->isDebugGPS());
  }
#endif

  y = 32;
  printStatus("compiled", __DATE__);
  printStatus("compiled", __TIME__);

  printStatus("DS3231", hal->hasDS3231() ? "OK" : "missing");
  printStatus("BMA400", hal->hasBMA400() ? "OK" : "missing");
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
  printStatus("BME280", hal->getPressure() != 0 ? "OK" : "missing");
  hal->updateMagnetometer();
  printStatus("QMC5883L", hal->getMagnetometerAzimuth() != 0 ? "OK" : "missing");
#endif
#ifdef BOARD_HAS_PSRAM
  printStatus("PSRAM", String(ESP.getPsramSize(), 10).c_str());
#endif
  printStatus("Temperature", String(hal->getTemperature() + String("C")).c_str());
  printStatus("Charging", hal->isCharging() ? "Yes" : "No");

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
  static uint8_t serialPtr = 0;

  printStatus("MicroSD present", hal->hasSD() ? "OK" : "missing");
  printStatus("MicroSD mounted", hal->isSDMounted() ? "OK" : "NO");
  printStatus("MicroSD (MB)",
              hal->isSDMounted() ? String(String((uint)(hal->sdCardSize() / (1024 * 1024))) + " MB").c_str() : "N/A");

  printStatus("GPS:", hal->hasGPS() ? "OK" : "missing");
  printStatus("Sattelites: ", String(hal->gpsNumSatellites()).c_str());

  printStatus("Latitude", String(hal->gpsLat()).c_str());
  printStatus("Longitude", String(hal->gpsLon()).c_str());
  if (!hal->isDebugGPS()) {
    printStatus("Button 1", hal->btnIsDown(BUTTON_1) ? "DOWN" : "UP");
    printStatus("Button 2", hal->btnIsDown(BUTTON_2) ? "DOWN" : "UP");
    printStatus("Button 3", hal->btnIsDown(BUTTON_3) ? "DOWN" : "UP");

    printStatus("Battery (Analog)", String(analogRead(B_MON)).c_str());
    // printStatus("Battery (Voltage)", (String(hal->getBatteryVoltage()) + " V").c_str());
  } else {
    while (hal->getSerialGPS().available()) {
      String line = hal->getSerialGPS().readStringUntil('\n');
      Serial.println(line);
      serialBuffer[serialPtr] = line;

      if (serialBuffer[serialPtr][0] == '$'     //
          && serialBuffer[serialPtr][1] == 'G'  //
          && serialBuffer[serialPtr][2] == 'P'  //
          && serialBuffer[serialPtr][3] == 'G'  //
          && serialBuffer[serialPtr][4] == 'L'  //
          && serialBuffer[serialPtr][5] == 'L') {
        Serial.print(serialPtr);
        Serial.println(">>>");
        serialPtr = 0;
      } else {
        serialPtr++;
        serialPtr = serialPtr >= SERIAL_BUF_SIZE ? 0 : serialPtr;
      }
    }

    hal->gfx()->setTextCursor(0, 120);
    for (uint8_t i = 0; i < SERIAL_BUF_SIZE; i++) {
      y += 8;
      hal->gfx()->setTextCursor(0, y);
      hal->gfx()->println(serialBuffer[i]);
    }
  }
#endif
  hal->requestFlush();
}

void OswAppPrintDebug::stop() {
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
  OswHal::getInstance()->gpsBackupMode();
  OswHal::getInstance()->stopMagnetometer();
#endif
}
