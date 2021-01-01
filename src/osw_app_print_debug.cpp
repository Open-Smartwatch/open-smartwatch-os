
#include "osw_app_print_debug.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <osw_pins.h>

#define SERIAL_BUF_SIZE 12

uint8_t y = 32;
uint8_t x = 52;
void printStatus(OswHal* hal, const char* setting, const char* value) {
  hal->getCanvas()->setCursor(x, y);
  hal->getCanvas()->print(setting);
  hal->getCanvas()->print(": ");
  hal->getCanvas()->print(value);
  y += 8;
}

void OswAppPrintDebug::loop(OswHal* hal) {
  static long loopCount = 0;
  static String serialBuffer[SERIAL_BUF_SIZE];
  static uint8_t serialPtr = 0;

  if (hal->btn2Down()) {
    hal->setDebugGPS(!hal->isDebugGPS());
  }
  
  loopCount++;
  hal->getCanvas()->fillRect(0, 0, 240, 240, rgb565(25, 25, 25));
  hal->getCanvas()->setTextColor(rgb565(200, 255, 200));

  y = 32;
  printStatus(hal, "compiled", __DATE__);
  printStatus(hal, "compiled", __TIME__);

  printStatus(hal, "DS3231", hal->hasDS3231() ? "OK" : "missing");
  printStatus(hal, "BMA400", hal->hasBMA400() ? "OK" : "missing");
  printStatus(hal, "BME280", hal->hasBME280() ? "OK" : "missing");
  printStatus(hal, "MicroSD present", hal->hasSD() ? "OK" : "missing");

  printStatus(hal, "MicroSD (MB)",
              hal->isSDMounted() ? String(String((uint)(hal->sdCardSize() / (1024*1024))) + " MB").c_str() : "N/A");

  printStatus(hal, "GPS:", hal->hasGPS() ? "OK" : "missing");
  printStatus(hal, "Sattelites: ", String(hal->gpsNumSatellites()).c_str());

  printStatus(hal, "Latitude", String(hal->gpsLat()).c_str());
  printStatus(hal, "Longitude", String(hal->gpsLon()).c_str());
  if (!hal->isDebugGPS()) {
    printStatus(hal, "Button 1", hal->btn1Down() ? "DOWN" : "UP");
    printStatus(hal, "Button 2", hal->btn2Down() ? "DOWN" : "UP");
    printStatus(hal, "Button 3", hal->btn3Down() ? "DOWN" : "UP");

    printStatus(hal, "Charging", hal->isCharging() ? "Yes" : "No");
    printStatus(hal, "Battery (Analog)", String(analogRead(B_MON)).c_str());
    printStatus(hal, "Battery (Voltage)", (String(hal->getBatteryVoltage()) + " V").c_str());
  } else {
    serialPtr = 0;
    while (hal->getSerialGPS().available()) {
      String line = hal->getSerialGPS().readStringUntil('\n');
      serialBuffer[serialPtr] = line;
      serialPtr++;
      serialPtr = serialPtr >= SERIAL_BUF_SIZE ? 0 : serialPtr;
    }

    hal->getCanvas()->setCursor(0, 120);
    for (uint8_t i = 0; i < SERIAL_BUF_SIZE; i++) {
      hal->getCanvas()->println(serialBuffer[i]);
    }
  }
}
