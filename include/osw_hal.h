#ifndef OSW_HAL_H
#define OSW_HAL_H

#include <Arduino.h>
#include <Arduino_TFT.h>
#include <gfx_2d.h>

#include "ArduinoGraphics2DCanvas.h"
#include "osw_app.h"

#define ERR_SD_MISSING 1
#define ERR_SD_MOUNT_FAILED 2

#define BTN_CLICK_TIMEOUT 333

class OswHal {
 public:
  // Constructor
  OswHal(void) {}

  // Setup
  void setupButtons(void);
  void setupDisplay(void);
  void setupGps(void);
  void setupPower(void);
  void setupSensors(void);
  uint8_t setupSD(void);

  // Buttons
  void checkButtons(void);

  // Display
  void setBrightness(uint8_t b);
  void increaseBrightness(uint8_t v);
  void decreaseBrightness(uint8_t v);

  Arduino_TFT* getArduino_TFT(void);
  ArduinoGraphics2DCanvas* getCanvas(void);
  void drawBuffer(Graphics2D* gfx2d);
  void flushCanvas(void);

  // SD
  void loadOsmTile(Graphics2D* target, int8_t z, float tilex, float tiley, int32_t offsetx, int32_t offsety);

  // GPS
  void redirectGpsToSerial(void);
  void parseGps(void);
  void gpsFullOnGps(void);
  void gpsFullOnGpsGlonassBeidu(void);

  // Power
  boolean isCharging(void);

  // Sensors
  float getPressureHPa();

  // Destructor
  ~OswHal(){};

 private:
  bool btn1Down;
  bool btn2Down;
  bool btn3Down;
  long lastBtn1Down = 0;
  long lastBtn2Down = 0;
  long lastBtn3Down = 0;
  uint8_t brightness;
};

#endif