
#include "osw_app_print_debug.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

// if you define PRINT_GPS the serial output
// is printed in the lower half of the screen

#ifdef PRINT_GPS
#define SERIAL_BUF_SIZE 10
#endif

void OswAppPrintDebug::run(OswHal* hal) {
#ifdef PRINT_GPS
  static String serialBuffer[SERIAL_BUF_SIZE];
  static uint8_t serialPtr = 0;
#endif

  static long loopCount = 0;
  loopCount++;
  hal->getCanvas()->fillRect(0, 0, 240, 240, rgb565(0, 0, 0));
  hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
  hal->getCanvas()->setCursor(52, 32);
  hal->getCanvas()->print("compiled:");
  hal->getCanvas()->setCursor(52, 40);
  hal->getCanvas()->print(__DATE__);
  hal->getCanvas()->print(" ");
  hal->getCanvas()->print(__TIME__);

  hal->getCanvas()->setCursor(52, 48);
  hal->getCanvas()->print("DS3231: ");
  hal->getCanvas()->print(hal->hasDS3231() ? "OK" : "missing");
  hal->getCanvas()->setCursor(52, 56);
  hal->getCanvas()->print("BMA400: ");
  hal->getCanvas()->print(hal->hasBMA400() ? "OK" : "missing");
  hal->getCanvas()->setCursor(52, 64);
  hal->getCanvas()->print("BME280: ");
  hal->getCanvas()->print(hal->hasBME280() ? "OK" : "missing");

  hal->getCanvas()->setCursor(52, 72);
  hal->getCanvas()->print("MicroSD present: ");
  hal->getCanvas()->print(hal->hasSD() ? "OK" : "missing");

  hal->getCanvas()->setCursor(52, 80);
  hal->getCanvas()->print("MicroSD mounted: ");
  if (hal->isSDMounted()) {
    hal->getCanvas()->print((uint32_t)(hal->sdCardSize() / 1024));
  }
  hal->getCanvas()->print(hal->isSDMounted() ? " MB" : "NO");

  hal->getCanvas()->setCursor(52, 88);
  hal->getCanvas()->print("GPS: ");
  hal->getCanvas()->print(hal->hasGPS() ? "OK" : "missing");

  hal->getCanvas()->setCursor(52, 96);
  hal->getCanvas()->print("Sattelites: ");
  hal->getCanvas()->print(hal->gpsNumSatellites());

  hal->getCanvas()->setCursor(52, 104);
  hal->getCanvas()->print("Fix: lat ");
  hal->getCanvas()->print(hal->gpsLat());
  hal->getCanvas()->print(" / lon");
  hal->getCanvas()->print(hal->gpsLon());

  hal->getCanvas()->setCursor(52, 112);
  hal->getCanvas()->print("Buttons: ");
  hal->getCanvas()->print(hal->btn1Down() ? "DOWN" : "UP");
  hal->getCanvas()->print(hal->btn2Down() ? "DOWN" : "UP");
  hal->getCanvas()->print(hal->btn3Down() ? "DOWN" : "UP");

#ifdef PRINT_GPS
  serialPtr = 0;
  while (hal->getSerialGPS().available()) {
    String line = hal->getSerialGPS().readStringUntil('\n');
    serialBuffer[serialPtr] = line;
    serialPtr++;
    serialPtr = serialPtr > SERIAL_BUF_SIZE ? 0 : serialPtr;
  }

  hal->getCanvas()->setCursor(0, 120);
  for (uint8_t i = 0; i < SERIAL_BUF_SIZE; i++) {
    hal->getCanvas()->println(serialBuffer[i]);
  }
#endif
}
