#ifndef OSW_HAL_H
#define OSW_HAL_H

#include <Arduino.h>

#include <gfx_2d.h>

#define ERR_SD_MISSING 1
#define ERR_SD_MOUNT_FAILED 2

class OswHal {
 public:
  OswHal(void) {}
  // setup
  void setupButtons(void);
  void setupDisplay(void);
  void setupGps(void);
  void setupPower(void);
  void setupSensors(void);
  uint8_t setupSD(void);

  // Display
  void setBrightness(uint8_t b);
  void increaseBrightness(uint8_t v);
  void decreaseBrightness(uint8_t v);

  // SD
  void loadOsmTile(Graphics2D* target, int8_t z, float tilex, float tiley, int32_t offsetx, int32_t offsety);
  // GPS
  void redirectGpsToSerial(void);
  void parseGps(void);
  void gpsFullOnGps(void);
  void gpsFullOnGpsGlonassBeidu(void);

  // Sensors
  ~OswHal(){};

 private:
  uint8_t brightness;
};

#endif