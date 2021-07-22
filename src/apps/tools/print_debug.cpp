
#include "./apps/tools/print_debug.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_pins.h>

#define SERIAL_BUF_SIZE 14

uint8_t y = 32;
uint8_t x = 52;
void printStatus(OswHal* hal, const char* setting, const char* value) {
  hal->gfx()->setTextCursor(x, y);
  hal->gfx()->print(setting);
  hal->gfx()->print(": ");
  hal->gfx()->print(value);
  y += 8;
}
void OswAppPrintDebug::setup(OswHal* hal) {
#if defined(GPS_EDITION)
  hal->gpsFullOnGpsGlonassBeidu();
  hal->setupCompass();
#endif
}
void OswAppPrintDebug::loop(OswHal* hal) {
  static long loopCount = 0;
  static String serialBuffer[SERIAL_BUF_SIZE];
  static uint8_t serialPtr = 0;

  if (hal->btnHasGoneDown(BUTTON_2)) {
#if defined(GPS_EDITION)
    hal->setDebugGPS(!hal->isDebugGPS());
#endif
  }

  loopCount++;
  hal->gfx()->fillFrame(0, 0, 240, 240, rgb565(25, 25, 25));
  hal->gfx()->setTextColor(rgb565(200, 255, 200), rgb565(0, 0, 0));
  hal->gfx()->setTextSize(1);

  y = 32;
  printStatus(hal, "compiled", __DATE__);
  printStatus(hal, "compiled", __TIME__);

  printStatus(hal, "DS3231", hal->hasDS3231() ? "OK" : "missing");
  printStatus(hal, "BMA400", hal->hasBMA400() ? "OK" : "missing");
#ifdef GPS_EDITION
  printStatus(hal, "BME280", hal->getPressure() != 0 ? "OK" : "missing");
  hal->updateCompass();
  printStatus(hal, "QMC5883L", hal->getCompassAzimuth() != 0 ? "OK" : "missing");
#endif
  printStatus(hal, "PSRAM", String(ESP.getPsramSize(), 10).c_str());

#if defined(GPS_EDITION)

  printStatus(hal, "MicroSD present", hal->hasSD() ? "OK" : "missing");
  printStatus(hal, "MicroSD mounted", hal->isSDMounted() ? "OK" : "NO");
  printStatus(hal, "MicroSD (MB)",
              hal->isSDMounted() ? String(String((uint)(hal->sdCardSize() / (1024 * 1024))) + " MB").c_str() : "N/A");

  printStatus(hal, "GPS:", hal->hasGPS() ? "OK" : "missing");
  printStatus(hal, "Sattelites: ", String(hal->gpsNumSatellites()).c_str());

  printStatus(hal, "Latitude", String(hal->gpsLat()).c_str());
  printStatus(hal, "Longitude", String(hal->gpsLon()).c_str());
  if (!hal->isDebugGPS()) {
    printStatus(hal, "Button 1", hal->btnIsDown(BUTTON_1) ? "DOWN" : "UP");
    printStatus(hal, "Button 2", hal->btnIsDown(BUTTON_2) ? "DOWN" : "UP");
    printStatus(hal, "Button 3", hal->btnIsDown(BUTTON_3) ? "DOWN" : "UP");

    printStatus(hal, "Charging", hal->isCharging() ? "Yes" : "No");
    printStatus(hal, "Battery (Analog)", String(analogRead(B_MON)).c_str());
    // printStatus(hal, "Battery (Voltage)", (String(hal->getBatteryVoltage()) + " V").c_str());
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

void OswAppPrintDebug::stop(OswHal* hal) {
#if defined(GPS_EDITION)
  hal->gpsBackupMode();
  hal->stopCompass();
#endif
}
