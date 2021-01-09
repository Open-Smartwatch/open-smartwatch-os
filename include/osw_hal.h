#ifndef OSW_HAL_H
#define OSW_HAL_H

#include <Arduino.h>
#include <Arduino_TFT.h>
#include <gfx_2d.h>

#include "ArduinoGraphics2DCanvas.h"
//#include "osw_app.h"

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
  bool btn1Down(void);
  bool btn2Down(void);
  bool btn3Down(void);

  // Display
  void setBrightness(uint8_t b);
  void increaseBrightness(uint8_t v);
  void decreaseBrightness(uint8_t v);
  void displayOff(void);
  void displayOn(void);

  Arduino_TFT* getArduino_TFT(void);
  ArduinoGraphics2DCanvas* getCanvas(void);
  void drawBuffer(Graphics2D* gfx2d);
  void flushCanvas(void);

  // SD
  void loadOsmTile(Graphics2D* target, int8_t z, float tilex, float tiley, int32_t offsetx, int32_t offsety);
  void loadPNG(Graphics2D* target, const char* path);
  void setPNGAlphaPlaceHolder(uint16_t color);
  bool hasSD(void);
  bool isSDMounted(void);
  uint64_t sdCardSize(void);
  void sdOff(void);

  // GPS
  HardwareSerial getSerialGPS(void);
  void gpsToSerial(void);
  void gpsParse(void);
  void gpsFullOnGps(void);
  void gpsFullOnGpsGlonassBeidu(void);
  void gpsAdvancedPowerSave(void);
  void gpsStandBy(void);
  void gpsBackupMode(void);
  void setDebugGPS(bool on);
  bool isDebugGPS();

  void gpsForceOn(boolean on);
  bool hasGPS(void);
  bool hasGPSFix(void);
  double gpsLat(void);
  double gpsLon(void);
  uint8_t gpsNumSatellites(void);

  // Power
  boolean isCharging(void);
  uint16_t getBatteryRaw(void);
  float getBatteryVoltage(void);
  void setCPUClock(uint8_t mhz);
  void deepSleep(long millis);

  // Sensors
  float getPressureHPa(void);
  bool hasBME280(void);
  bool hasBMA400(void);
  bool hasDS3231(void);
  void updateAccelerometer(void);
  float getAccelerationX(void);
  float getAccelerationY(void);
  float getAccelerationZ(void);
  uint32_t getStepCount(void);
  void enableStepDetection(void);
  void disableStepDetection(void);
  uint8_t getActivityMode(void);
  uint32_t getTime(void);

  // Destructor
  ~OswHal(){};

 private:
  bool _btn1Down = false;
  bool _btn2Down = false;
  bool _btn3Down = false;
  long _lastBtn1Down = 0;
  long _lastBtn2Down = 0;
  long _lastBtn3Down = 0;
  uint8_t _brightness = 0;
  bool _hasBME280 = false;
  bool _hasBMA400 = false;
  bool _hasDS3231 = false;
  bool _hasSD = false;
  bool _isSDMounted = false;
  bool _hasGPS = false;
  bool _debugGPS = false;
};

#endif