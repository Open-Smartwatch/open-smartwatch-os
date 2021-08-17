#ifndef OSW_HAL_H
#define OSW_HAL_H

#include <Arduino.h>
#include <Arduino_TFT.h>
#include <gfx_2d_print.h>
#include <Wire.h>

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

/**
 * @brief After how many milliseconds the press is a long press ?
 */
#define DEFAULTLAUNCHER_LONG_PRESS 1000

/**
 * @brief Define the number of button to monitor
 *
 */
#define NUM_BUTTONS 3

/**
 * @brief Index of the different buttons.
 *
 *    - BUTTON_1 is the bottom left button
 *    - BUTTON_2 is the top right button
 *    - BUTTON_3 is the bottom right button
 *
 */
enum Button { BUTTON_1 = 0, BUTTON_2 = 1, BUTTON_3 = 2 };

/**
 * @class OswHal
 *
 * @brief Hardware Astraction Layer
 *
 * This class is the layer to communicate with the ESP32 hardware.
 * You will find all the function to monitor the hardware like get information from the RTC,
 * get information from the button, manage the accelerometer, etc.
 *
 */
class OswHal {
 public:
  /**
   * @brief Initialise the communication with the ESP32.
   *
   * @param fs file system to use on the ESP.
   */
  OswHal(FileSystemHal* fs) : fileSystem(fs) {
    // begin I2c communication
    Wire.begin(SDA, SCL, 100000L);
  }

  /**
   * -------------- GLOBAL SETUP ------------
   */

  /**
   * @brief We check the file system to be sure it can be used.
   *
   */
  void setupFileSystem(void);


  /**
   * @brief Initialize the buttons.
   *
   */
  void setupButtons(void);


  /**
   * @brief Hardware initialisation of the display.
   *
   * The display is configured and turned on.
   *
   */
  void setupDisplay(void);


  /**
   * @brief Hardware initialisation of the battery
   *
   */
  void setupPower(void);


  /**
   * @brief  Hardware initialisation of the Accelerometer
   *
   */
  void setupAccelerometer(void);

  /**
   * @brief Hardware initialisation of the clock
   *
   */
  void setupTime(void);


#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
  void setupEnvironmentSensor(void);
  void stopEnvironmentSensor(void);
  void setupMagnetometer(void);
  void stopMagnetometer(void);
  void setupGps(void);
#endif

  /**  
   * -------------- BUTTONS MANAGEMENT ------------
   *
   * There is 3 buttons.
   *
   * Each of them could be
   *   - up,
   *   - down,
   *   - GoneDone (it was up and is just pressed to be down)
   *   - GoneUp (it was down and is just released to be up)
   *
   * If the button is pressed for a long time (DEFAULTLAUNCHER_LONG_PRESS) a long-pressed state could be get.
   *
   *
   */
  /**
   * @brief Checking all the buttons and store their states (up or down).
   *
   * The state of each button will be accessible through the buttons functions.
   *
   */
  void checkButtons(void);


  /**
   * @brief Check if the button just gone down
   *
   * @param btn Index of the button
   * @return true when the button is gone down
   * @return false
   */
  bool btnHasGoneDown(Button btn);


  /**
   * @brief Check if the button just gone up
   *
   * @param btn
   * @return true
   * @return false
   */
  bool btnHasGoneUp(Button btn);


  /**
   * @brief Check if the button has been pressed and is down or not.
   *
   * @param btn Button index
   * @return true The button is pressed
   * @return false The button is not pressed
   */
  bool btnIsDown(Button btn);


  /**
   * @brief Check if the button btn has been pressed for a long time or not.
   *
   * @param btn Button index
   * @return true The button is pressed for a long time.
   * @return false The button is not pressed for a long time.
   */
  bool btnIsLongPress(Button btn);


  void suppressButtonUntilUp(Button btn);

  /**
   * @brief Give the time (in millisecond) since the button is pressed.
   *
   * @param btn Button index
   * @return unsigned long time in milliseconds
   */
  unsigned long btnIsDownSince(Button btn);


  /**
   * @brief Initialize the state of a button.
   *
   * The button is not GoneUp, GoneDown or LongPress
   *
   * @param btn Index of the button
   */
  void clearButtonState(Button btn);


#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
  void vibrate(long millis);
#endif


  /**
   *  -------------- DISPLAY MANAGEMENT ------------
   */

  /**
   * @brief Set the screen brightness at the value v
   *
   * Max value is 255 and min value is 0.
   *
   * @param b
   */
  void setBrightness(uint8_t b);

  /**
   * @brief Increase the screen brightness of the value v
   *
   * Max value is 255 and min value is 0.
   *
   * @param v Brightness to increase
   */
  void increaseBrightness(uint8_t v);

  /**
   * @brief Decrease the screen brightness of the value v
   *
   * Max value is 255 and min value is 0.
   *
   * The minimal brightness defined in the configuration is applied if defined.
   *
   * @param v Brightness to decrease
   */
  void decreaseBrightness(uint8_t v);

  /**
   * @brief Turn off the display.
   */
  void displayOff(void);

  /**
   * @brief Turn on the display.
   */
  void displayOn(void);

  void requestDisableDisplayBuffer();
  void requestEnableDisplayBuffer();
  void disableDisplayBuffer();
  void enableDisplayBuffer();

  /**
   * @brief Returns the time since the screen is on.
   *
   * @return unsigned long Time in milliseconds
   */
  unsigned long screenOnTime();

  /**
   * @brief Returns the time since the screen is off.
   *
   * @return unsigned long Time in milliseconds
   */
  unsigned long screenOffTime();

  /**
   * @brief Get the screen brightness from 0 to 255.
   *
   * @return uint8_t
   */
  uint8_t screenBrightness();

  Arduino_TFT* getArduino_TFT(void);
  Arduino_Canvas_Graphics2D* getCanvas(void);
  Graphics2DPrint* gfx(void);

  /**
   * @brief Flush the buffer to refresh the screen
   * 
   */
  void flushCanvas(void);

  /**
   * @brief Ask to flush the buffer
   *
   */
  void requestFlush(void);

  /**
   * @brief Does something asked to flush the buffer ?
   *
   * @return true
   * @return false
   */
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

  /**
   * @brief Does the watch is equipped of a GPS ?
   *
   * @return true Yes the watch has a GPS
   * @return false No the watch has no GPS
   */
  bool hasGPS(void);

  bool hasGPSFix(void);
  gps_fix* gpsFix(void);
  double gpsLat(void);
  double gpsLon(void);
  uint8_t gpsNumSatellites(void);
  NMEAGPS* gps(void);

#endif

  /**
   * -------------- POWER MANAGEMENT ------------
   * 
   */

  /**
   * @brief Check if charging
   *
   * @return boolean Is charging = true
   */
  boolean isCharging(void);


  /**
   * @brief Get the Battery raw charge informations
   *
   * @return uint16_t raw charge information from 0 to 31 (100%)
   */
  uint16_t getBatteryRaw(void);


  /**
   * Get an approximate percentage of the battery charge from 0 to 100
   *
   * @return uint8_t Percentage of the battery (0-100)
   **/
  uint8_t getBatteryPercent(void);

  void setCPUClock(uint8_t mhz);

  /**
   * @brief Put the watch to a deep sleep.
   *
   * @param millis time (in milliseconds) before the watch could be wake-up.
   * @param wakeFromButtonOnly Does the wake-up is only done with the button.
   */
  void deepSleep(long millis, bool wakeFromButtonOnly = false);

  /**
   * @brief Put the watch to a light sleep.
   *
   * @param millis time (in milliseconds) before the watch could be wake-up.
   */
  void lightSleep(long millis);

  /**
   * @brief Put the watch to a light sleep.
   *
   */
  void lightSleep();

  /**
   * @brief Wake-up the watch after a light sleep
   *
   */
  void handleWakeupFromLightSleep();

  /**
   * -------------- SENSOR MANAGEMENT ------------
   */

  bool hasBMA400(void);
  bool hasDS3231(void);
  void updateAccelerometer(void);

  /**
   * @brief Reset of the accelerometer.
   *
   * The step counter is reset to 0 and the hardware is reset.
   *
   */
  void resetAccelerometer(void);


  /**
   * @brief Hardware initialisation of the accelerometer..
   *
   */
  void initAccelerometer(void);


  /**
   * @brief Get the Acceleration of X axis
   *
   * @return float X acceleration in square meters per second
   */
  float getAccelerationX(void);


  /**
   * @brief Get the Acceleration of Y axis
   *
   * @return float Y acceleration in square meters per second
   */
  float getAccelerationY(void);


  /**
   * @brief Get the Acceleration of Z axis.
   *
   * @return float Z acceleration in square meters per second
   */
  float getAccelerationZ(void);


  /**
   * @brief Get the step count from the accelerometer.
   *
   * @return uint32_t Step count from the last reset of the accelerometer.
   */
  uint32_t getAccelStepCount(void);


  /**
   * @brief Get the current activity type : STILL/WALK/RUN
   *
   * @return uint8_t Activity type code.
   */
  // TODO : check this point
  uint8_t getActivityMode(void);

  // Statistics: Steps
  uint32_t getStepsToday(void);
  uint32_t getStepsOnDay(uint8_t dayOfWeek);
  uint32_t getStepsTotal(void);

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
  void updateEnvironmentSensor(void);
  void updateMagnetometer(void);
  float getTemperature(void);
  float getPressure(void);
  float getHumidity(void);
  byte getMagnetometerBearing(void);
  int getMagnetometerAzimuth(void);
  int getMagnetometerX(void);
  int getMagnetometerY(void);
  int getMagnetometerZ(void);
  void setMagnetometerCalibration(int x_min, int x_max, int y_min, int y_max, int z_min, int z_max);
#endif

  /**
   * -------------- TIME MANAGEMENT ------------
   */

  /**
   * @brief Set the current time to the RTC (Real Time Clock) controller.
   *
   */
  void setUTCTime(long);


  /**
   * @brief Try to get the current time from the RTC (Real Time Clock) controller.
   *
   * There are several way to get the current time. This function try each of them until the
   * time is fetched.
   *
   */
  void updateRtc(void);


  /**
   * @brief Get the current time UTC
   *
   * @return uint32_t Number of milliseconds since 1/1/1970
   */
  uint32_t getUTCTime(void);


  /**
   * @brief Get Hour, Minute and Second since 1/1/1970
   *
   * @param hour Number of hours
   * @param minute Number of minutes
   * @param second Number of seconds
   */
  void getUTCTime(uint32_t* hour, uint32_t* minute, uint32_t* second);


  /**
   * Get the local time based on the time zone in the config file
   *
   * @return uint32_t Number of second since 1/1/1970
   */
  uint32_t getLocalTime(void);


  /**
   * @brief Get the hour, minute and second of the local time
   *
   * @param hour hour from 1 to 12
   * @param minute minute from 0 to 59
   * @param second second from 0 to 59
   */
  void getLocalTime(uint32_t* hour, uint32_t* minute, uint32_t* second);


  /**
   * @brief Get the hour, minute and second of the local time
   *
   * @param hour return hour from 1 to 12
   * @param minute return minute from 0 to 59
   * @param second return second from 0 to 59
   * @param afternoon return true if the time is PM
   */
  void getLocalTime(uint32_t* hour, uint32_t* minute, uint32_t* second, bool* afterNoon);


  /**
   * @brief Get the current date informations
   *
   * @param day return the day value from 1 to 31
   * @param weekDay return the day of week (0 : Sunday, 1 : Monday, etc.)
   */
  void getDate(uint32_t* day, uint32_t* weekDay);


  /**
   * @brief Get current date informations
   *
   * @param day return the day value from 1 to 31
   * @param month return the month number from 1 to 12
   * @param year return the year number. Ex : 2021
   */
  void getDate(uint32_t* day, uint32_t* month, uint32_t* year);


  /**
   * @brief Get the x first chars of the week day name of the current date.
   *
   * Example : getWeekdayString(4, string *output)
   *
   * @param firstNChars
   * @param output
   */
  const char* getWeekday(void);


  // Destructor
  ~OswHal(){};

  bool _requestDisableBuffer = false;
  bool _requestEnableBuffer = false;
  Button buttons[NUM_BUTTONS] = {BUTTON_1, BUTTON_2, BUTTON_3};

 private:
  /**
   * @brief UTC time of the RTC controller.
   *
   */
  uint32_t _utcTime = 0;


  /**
   * @brief Since how many milliseconds the screen is ON ?
   *
   */
  unsigned long _screenOnSince;


  /**
   * @brief @brief Since how many milliseconds the screen is OFF ?
   *
   */
  unsigned long _screenOffSince;


  /**
   * @brief Does the last state of the button was down ?
   *
   */
  bool _btnLastStateWasDown[NUM_BUTTONS];


  /**
   * @brief Does the button is down or not ?
   *
   */
  bool _btnIsDown[NUM_BUTTONS];


  /**
   * @brief Does the button just gone up ?
   *
   */
  bool _btnGoneUp[NUM_BUTTONS];


  bool _btnSuppressUntilUpAgain[NUM_BUTTONS];

  /**
   * @brief Does the button just gone down ?
   *
   */
  bool _btnGoneDown[NUM_BUTTONS];


  /**
   * @brief Since how many time (in milliseconds) the button is down ?
   */
  unsigned long _btnIsDownMillis[NUM_BUTTONS];


  /**
   * @brief Does the button is pressed since a long time ?
   */
  bool _btnLongPress[NUM_BUTTONS];


  /**
   * @brief Brightness of the screen (0-255)
   */
  uint8_t _brightness = 0;


  bool _hasBMA400 = false;

  /**
   * @brief Does the watch is equipped of a GPS ?
   *
   */
  bool _hasGPS = false;


  bool _debugGPS = false;
  bool _requestFlush = false;
  bool _isLightSleep = false;

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
  bool _hasBME280 = false;

  /**
   * @brief Temperature
   *
   */
  // TODO What is the unit used ?
  float _temp = -100;

  /**
   * @brief Humidity
   */
  // TODO What is the unit used ?
  float _hum = -100;

  /**
   * @brief Atmospherical pressure.
   */
  // TODO What is the unit used ?
  float _pres = -100;
#endif

  FileSystemHal* fileSystem;
};

#endif
