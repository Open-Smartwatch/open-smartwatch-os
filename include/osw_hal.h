#ifndef OSW_HAL_H
#define OSW_HAL_H

#include <Arduino.h>
#include <Arduino_TFT.h>
#include <gfx_2d_print.h>
#include <Wire.h>
#include <Preferences.h>

#include "Arduino_Canvas_Graphics2D.h"
#include "hal/osw_filesystem.h"
#include "osw_config_keys.h"
#include "osw_pins.h"
//#include "osw_app.h"
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
#include <NMEAGPS.h>
#endif

#define ERR_SD_MISSING 1
#define ERR_SD_MOUNT_FAILED 2

#define DEFAULTLAUNCHER_LONG_PRESS 1000
#define NUM_BUTTONS 3
// enum for user space button handling
enum Button { BUTTON_1 = 0, BUTTON_2 = 1, BUTTON_3 = 2 };

class OswHal {
 public:
  static OswHal* getInstance();

  // Setup
  void setup(bool fromLightSleep);
  void setupFileSystem(void);
  void setupButtons(void);
  void setupDisplay();
  void setupPower();
  void setupAccelerometer();
  void setupTime(void);
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
  void setupEnvironmentSensor(void);
  void setupMagnetometer(void);
  void setupGps(void);
#endif

  // Stop
  void stop(bool toLightSleep);
  void stopPower();
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
  void stopEnvironmentSensor(void);
  void stopMagnetometer(void);
#endif

  // Buttons
  void checkButtons(void);
  // long btn1Down(void);
  // long btn2Down(void);
  // long btn3Down(void);
  // void clearBtn1(void);
  // void clearBtn2(void);
  // void clearBtn3(void);

  // Buttons (Engine-Style)
  bool btnHasGoneDown(Button btn);
  bool btnHasGoneUp(Button btn);
  bool btnIsDown(Button btn);
  bool btnIsLongPress(Button btn);
  void suppressButtonUntilUp(Button btn);
  unsigned long btnIsDownSince(Button btn);
  void clearButtonState(Button btn);
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)

  void vibrate(long millis);
#endif

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

  /**
   * @brief Get the screen brightness from 0 to 255.
   * 
   * @param bool checkHardware If true, we check the brightness on the hardware before return it.
   * @return uint8_t
   */
  uint8_t screenBrightness(bool checkHardware = false);

  Arduino_TFT* getArduino_TFT(void);
  Arduino_Canvas_Graphics2D* getCanvas(void);
  Graphics2DPrint* gfx(void);
  void flushCanvas(void);
  void requestFlush(void);
  bool isRequestFlush(void);
  void loadPNGfromProgmem(Graphics2D* target, const unsigned char* array, unsigned int length);

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)

  // SD
  void loadOsmTile(Graphics2D* target, int8_t z, float tilex, float tiley, int32_t offsetx, int32_t offsety);
  void loadPNGfromSD(Graphics2D* target, const char* path);
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
  gps_fix* gpsFix(void);
  double gpsLat(void);
  double gpsLon(void);
  uint8_t gpsNumSatellites(void);
  NMEAGPS* gps(void);

#endif
  // Power
  boolean isCharging(void);
  uint16_t getBatteryRaw(const uint16_t numAvg = 8);
  // float getBatteryVoltage(void);
  void updatePowerStatistics(uint16_t currBattery);
  uint8_t getBatteryPercent();
  void setCPUClock(uint8_t mhz);
  void deepSleep(long millis = 0);
  void lightSleep(long millis = 0);
  void handleWakeupFromLightSleep();

  // Sensors
  bool hasBMA400(void);
  bool hasDS3231(void);
  void updateAccelerometer(void);
  void resetAccelerometer(void);
  void initAccelerometer(void);
  float getAccelerationX(void);
  float getAccelerationY(void);
  float getAccelerationZ(void);
  uint32_t getAccelStepCount(void);
  uint8_t getActivityMode(void);
  // Statistics: Steps
  uint32_t getStepsToday(void);
  uint32_t getStepsOnDay(uint8_t dayOfWeek);
  uint32_t getStepsTotal(void);

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
  void updateEnvironmentSensor(void);
  void updateMagnetometer(void);
  float getPressure(void);
  float getHumidtiy(void);
  byte getMagnetometerBearing(void);
  int getMagnetometerAzimuth(void);
  int getMagnetometerX(void);
  int getMagnetometerY(void);
  int getMagnetometerZ(void);
  void setMagnetometerCalibration(int x_min, int x_max, int y_min, int y_max, int z_min, int z_max);
#endif
  float getTemperature(); // Get the temperature either by the sensor of the GPS edition or from the RTC module

  // Time
  void setUTCTime(long);
  void updateRtc(void);
  uint32_t getUTCTime(void);
  void getUTCTime(uint32_t* hour, uint32_t* minute, uint32_t* second);
  uint32_t getLocalTime(void);
  void getLocalTime(uint32_t* hour, uint32_t* minute, uint32_t* second);
  void getLocalTime(uint32_t* hour, uint32_t* minute, uint32_t* second, bool* afterNoon);
  void getDate(uint32_t* day, uint32_t* weekDay);
  void getDate(uint32_t* day, uint32_t* month, uint32_t* year);
  const char* getWeekday(void);
  // Destructor
  ~OswHal(){};

  bool _requestDisableBuffer = false;
  bool _requestEnableBuffer = false;
  Button buttons[NUM_BUTTONS] = {BUTTON_1, BUTTON_2, BUTTON_3};

 private:
  // Constructor
  OswHal(FileSystemHal* fs) : fileSystem(fs) {
      //begin I2c communication
      Wire.begin(SDA, SCL, 100000L);
  }

  static OswHal* instance;
  uint32_t _utcTime = 0;
  unsigned long _screenOnSince;
  unsigned long _screenOffSince;
  // array of avaialble buttons for iteration (e.g. handling)
  bool _btnLastState[NUM_BUTTONS];
  bool _btnIsDown[NUM_BUTTONS];
  bool _btnGoneUp[NUM_BUTTONS];
  bool _btnSuppressUntilUpAgain[NUM_BUTTONS];
  bool _btnGoneDown[NUM_BUTTONS];
  unsigned long _btnIsDownMillis[NUM_BUTTONS];
  bool _btnLongPress[NUM_BUTTONS];
  long _lastTap = 0;
  long _lastDoubleTap = 0;
  uint8_t _brightness = 0;
  bool _hasBMA400 = false;
  bool _hasGPS = false;
  bool _debugGPS = false;
  bool _requestFlush = false;
  bool _isLightSleep = false;
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)

  bool _hasBME280 = false;
  float _temp = -100;
  float _hum = -100;
  float _pres = -100;
#endif

  Preferences powerStatistics;
  FileSystemHal* fileSystem;

  uint16_t getBatteryRawMin();
  uint16_t getBatteryRawMax();
};

#endif
