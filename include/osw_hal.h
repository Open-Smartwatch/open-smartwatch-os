#ifndef OSW_HAL_H
#define OSW_HAL_H

#include <Arduino.h>
#include <Arduino_TFT.h>
#include <gfx_2d_print.h>
#include <mini-wifi.h>
#include <string>
using std::string;

#include "ArduinoGraphics2DCanvas.h"
#include "hal/osw_filesystem.h"
//#include "osw_app.h"

#define ERR_SD_MISSING 1
#define ERR_SD_MOUNT_FAILED 2

#define BTN_CLICK_TIMEOUT 333

enum PhysicalInput {
  BUTTON_1 = 0,
  BUTTON_2 = 1,
  BUTTON_3 = 2
};

class OswHal {
 public:
  // Constructor
  OswHal(FileSystemHal *fs): fileSystem(fs) {}

  // Setup
  void setupFileSystem(void);
  void setupButtons(void);
  void setupDisplay(void);
  void setupPower(void);
  void setupSensors(void);
  void setupTime(void);
#if defined(GPS_EDITION)
  uint8_t setupSD(void);
  void setupGps(void);
#endif

  // Buttons
  void checkButtons(void);
  long btn1Down(void);
  long btn2Down(void);
  long btn3Down(void);
  void clearBtn1(void);
  void clearBtn2(void);
  void clearBtn3(void);
  
  // Buttons (Engine-Style)
  bool btnHasGoneDown(uint8_t btn);
  bool btnHasGoneUp(uint8_t btn);
  bool btnIsPhysicalDown(uint8_t btn);
  bool btnIsLongPress(uint8_t btn);
  void suppressButtonUntilUp(uint8_t btn);

  





  // Display
  void setBrightness(uint8_t b);
  void increaseBrightness(uint8_t v);
  void decreaseBrightness(uint8_t v);
  void displayOff(void);
  void displayOn(void);
  void requestDisableDisplayBuffer();
  void requestEnableDisplayBuffer();
  void disableDisplayBuffer();
  void enableDisplayBuffer();
  unsigned long screenOnTime();
  unsigned long screenOffTime();
  uint8_t screenBrightness();

  Arduino_TFT* getArduino_TFT(void);
  ArduinoGraphics2DCanvas* getCanvas(void);
  Graphics2DPrint* gfx(void);
  void flushCanvas(void);
  void requestFlush(void);
  bool isRequestFlush(void);

#if defined(GPS_EDITION)

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

#endif
  // Power
  boolean isCharging(void);
  uint16_t getBatteryRaw(void);
  // float getBatteryVoltage(void);
  uint8_t getBatteryPercent(void);
  void setCPUClock(uint8_t mhz);
  void deepSleep(long millis);
  void deepSleep();

  // Sensors
  bool hasBMA400(void);
  bool hasDS3231(void);
  void updateAccelerometer(void);
  float getAccelerationX(void);
  float getAccelerationY(void);
  float getAccelerationZ(void);
  uint32_t getStepCount(void);
  uint8_t getActivityMode(void);

  // Time
  void updateTimeViaNTP(long gmtOffset_sec, int daylightOffset_sec, uint32_t timeout_sec);
  void setUTCTime(long);
  long getUTCTime(void);
  void getUTCTime(uint32_t* hour, uint32_t* minute, uint32_t* second);
  long getLocalTime(void);
  void getLocalTime(uint32_t* hour, uint32_t* minute, uint32_t* second);
  void getLocalTime(uint32_t* hour, uint32_t* minute, uint32_t* second, bool *afterNoon);
  void getDate(uint32_t *day, uint32_t *weekDay);
  void getDate(uint32_t *day, uint32_t *month, uint32_t *year);
  void getWeekdayString(int firstNChars, string *output);

  // RF
  MiniWifi* getWiFi(void);

  // Destructor
  ~OswHal(){};

  bool _requestDisableBuffer = false;
  bool _requestEnableBuffer = false;

 private:
  unsigned long _screenOnSince;
  unsigned long _screenOffSince;
  long _btn1DownSince = 0;
  long _btn2DownSince = 0;
  long _btn3DownSince = 0;
  long _btn1UpSince = 0;
  long _btn2UpSince = 0;
  long _btn3UpSince = 0;
  bool btnLastState[3];
  bool btnIsDown[3];
  bool btnGoneUp[3];  
  bool btnSuppressUntilUpAgain[3];  
  bool btnGoneDown[3];   
  unsigned long btnIsDownSince[3];  
  bool btnLongPress[3];  
  long _lastTap = 0;
  long _lastDoubleTap = 0;
  uint8_t _brightness = 0;
  bool _hasBMA400 = false;
  bool _hasSD = false;
  bool _isSDMounted = false;
  bool _hasGPS = false;
  bool _debugGPS = false;
  bool _requestFlush = false;

  FileSystemHal *fileSystem;
};

#endif
