#ifndef OSW_HAL_H
#define OSW_HAL_H

#include <memory>

#include <Arduino.h>
#ifdef OSW_EMULATOR
#include <FakeMe.h>
#else
#include <Arduino_TFT.h>
#endif
#include "Arduino_Canvas_Graphics2D.h"
#include <Wire.h>
#include <Preferences.h>

#include OSW_TARGET_PLATFORM_HEADER
#include "hal/osw_filesystem.h"
#include <devices/interfaces/OswTimeProvider.h>
#include "osw_config_keys.h"
#include "osw_pins.h"
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
#include <NMEAGPS.h>
#endif

#define ERR_SD_MISSING 1
#define ERR_SD_MOUNT_FAILED 2

#define NUM_BUTTONS 3
// enum for user space button handling
enum Button { BUTTON_1 = 0, BUTTON_2 = 1, BUTTON_3 = 2 };

class OswHal {
  public:
    static OswHal* getInstance();
    static void resetInstance();

    class Devices;
    std::unique_ptr<Devices> devices = nullptr;

#if OSW_PLATFORM_ENVIRONMENT == 1
    class Environment;
    std::unique_ptr<Environment> environment = nullptr;
#endif

    // Setup
    void setup(bool fromLightSleep);
    void setupFileSystem(void);
    void setupButtons(void);
    void setupDisplay();
    void setupPower();
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
    void setupGps(void);
#endif

    // Stop
    void stop(bool toLightSleep);
    void stopPower();

    // Buttons (Engine-Style)
    void checkButtons(void);
    bool btnHasGoneDown(Button btn);
    bool btnIsDoubleClick(Button btn);
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
    void setBrightness(uint8_t b, bool storeToNVS = true);
    void increaseBrightness(uint8_t v);
    void decreaseBrightness(uint8_t v);
    void displayOff();
    void displayOn();
    void requestDisableDisplayBuffer();
    void requestEnableDisplayBuffer();
    void disableDisplayBuffer();
    void enableDisplayBuffer();
    bool displayBufferEnabled();
    unsigned long screenOnTime();
    unsigned long screenOffTime();

    /**
     * @brief Get the screen brightness from 0 to 255.
     *
     * @param bool checkHardware If true, we check the brightness on the hardware before return it.
     * @return uint8_t
     */
    uint8_t screenBrightness(bool checkHardware = false);

    Arduino_Canvas_Graphics2D* getCanvas(void);
    Graphics2DPrint* gfx();
    void flushCanvas();
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
    bool isCharging(void);
    uint16_t getBatteryRaw(const uint16_t numAvg = 8);
    // float getBatteryVoltage(void);
    void updatePowerStatistics(uint16_t currBattery);
    uint8_t getBatteryPercent();
    void setCPUClock(uint8_t mhz);
    uint8_t getCPUClock();
    void deepSleep(long millis = 0);
    void lightSleep(long millis = 0);
    void handleWakeupFromLightSleep();

    // General time stuff
    void updateTimeProvider();
    void updateTimezoneOffsets();

    // UTC Time
    void setUTCTime(const time_t& epoch);
    time_t getUTCTime();
    void getUTCTime(uint32_t* hour, uint32_t* minute, uint32_t* second);

    // Offset getters for primary / secondary time (cached!)
    time_t getTimezoneOffsetPrimary();
    time_t getTimezoneOffsetSecondary();

    // New time functions with offset
    time_t getTime(time_t& offset);
    void getTime(time_t& offset, uint32_t* hour, uint32_t* minute, uint32_t* second, bool* afterNoon = nullptr);
    void getDate(time_t& offset, uint32_t* day, uint32_t* weekDay);
    void getDate(time_t& offset, uint32_t* day, uint32_t* month, uint32_t* year);
    const char* getWeekday(time_t& offset, uint32_t* setWDay = nullptr);

    // For backward compatibility: Local time functions (= primary timezone)
    inline void getLocalTime(uint32_t* hour, uint32_t* minute, uint32_t* second, bool* afterNoon = nullptr) {
        this->getTime(this->timezoneOffsetPrimary, hour, minute, second, afterNoon);
    }
    inline uint32_t getLocalTime() {
        return this->getTime(this->timezoneOffsetPrimary);
    }
    inline void getLocalDate(uint32_t* day, uint32_t* weekDay) {
        this->getDate(this->timezoneOffsetPrimary, day, weekDay);
    };
    inline void getLocalDate(uint32_t* day, uint32_t* month, uint32_t* year) {
        this->getDate(this->timezoneOffsetPrimary, day, month, year);
    };
    inline const char* getLocalWeekday(uint32_t* sWDay = nullptr) {
        return this->getWeekday(this->timezoneOffsetPrimary, sWDay);
    };

    // For backward compatibility: Dual time functions (= secondary timezone)
    inline void getDualTime(uint32_t* hour, uint32_t* minute, uint32_t* second, bool* afterNoon = nullptr) {
        this->getTime(this->timezoneOffsetSecondary, hour, minute, second, afterNoon);
    }
    inline uint32_t getDualTime() {
        return this->getTime(this->timezoneOffsetSecondary);
    }
    inline void getDualDate(uint32_t* day, uint32_t* weekDay) {
        this->getDate(this->timezoneOffsetSecondary, day, weekDay);
    };
    inline void getDualDate(uint32_t* day, uint32_t* month, uint32_t* year) {
        this->getDate(this->timezoneOffsetSecondary, day, month, year);
    };
    inline const char* getDualWeekday(uint32_t* sWDay = nullptr) {
        return this->getWeekday(this->timezoneOffsetSecondary, sWDay);
    };

    bool _requestDisableBuffer = false;
    bool _requestEnableBuffer = false;
    Button buttons[NUM_BUTTONS] = {BUTTON_1, BUTTON_2, BUTTON_3};

  private:
    // Constructor
    OswHal(FileSystemHal* fs);
    ~OswHal();

    Arduino_Canvas_Graphics2D* canvas = nullptr;

    static OswHal* instance;
    OswTimeProvider* timeProvider = nullptr;
    unsigned long _screenOnSince;
    unsigned long _screenOffSince;
    // array of available buttons for iteration (e.g. handling)
    bool _btnLastState[NUM_BUTTONS];
    bool _btnIsDown[NUM_BUTTONS];
    bool _btnGoneUp[NUM_BUTTONS];
    bool _btnSuppressUntilUpAgain[NUM_BUTTONS];
    bool _btnGoneDown[NUM_BUTTONS];
    unsigned long _btnIsDownMillis[NUM_BUTTONS];

    bool _btnDoubleClickTimeout[NUM_BUTTONS];
    unsigned long _btnDoubleClickMillis[NUM_BUTTONS];
    bool _btnDoubleClick[NUM_BUTTONS];
    uint8_t _btnDetectDoubleClickCount[NUM_BUTTONS];

    bool _btnLongPress[NUM_BUTTONS];
    long _lastTap = 0;
    long _lastDoubleTap = 0;
    uint8_t _brightness = 0;
    bool _hasGPS = false;
    bool _debugGPS = false;
    bool _isLightSleep = false;

    time_t timezoneOffsetPrimary = 0;
    time_t timezoneOffsetSecondary = 0;

    Preferences powerStatistics;
    FileSystemHal* fileSystem;

    uint16_t getBatteryRawMin();
    uint16_t getBatteryRawMax();
};

#endif

#include "hal/devices.h"
#include "hal/environment.h"