#include <osw_hal.h>
#include <services/OswServiceManager.h>

#ifndef OSW_EMULATOR
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
#include "hal/esp32/sd_filesystem.h"
#else
#include "hal/esp32/spiffs_filesystem.h"
#endif
#endif

OswHal* OswHal::instance = nullptr;

OswHal* OswHal::getInstance() {
    if(OswHal::instance == nullptr) {
#ifndef OSW_EMULATOR
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
        OswHal::instance = new OswHal(new SDFileSystemHal());
#else
        OswHal::instance = new OswHal(new SPIFFSFileSystemHal());
#endif
#else
        OswHal::instance = new OswHal(nullptr);
#endif
    }
    return OswHal::instance;
};

void OswHal::resetInstance() {
    delete OswHal::instance;
    OswHal::instance = nullptr; // On the next access it will be recreated!
}

OswHal::OswHal(FileSystemHal* fs) : fileSystem(fs) {
    //begin I2c communication
#ifndef OSW_EMULATOR
#if defined(OSW_DEVICE_I2C_SDA) && defined(OSW_DEVICE_I2C_SCL)
    bool res = Wire.begin(OSW_DEVICE_I2C_SDA, OSW_DEVICE_I2C_SCL, 100000L);
    assert(res);
#else
#warning "I2C pins are not set on this platform, ignoring it then..."
#endif
#endif
}

OswHal::~OswHal() {

};

void OswHal::setup(bool fromLightSleep) {
    if(!fromLightSleep) {
        // Handle full boot from deep sleep
        this->timeProvider = nullptr; // He is properly destroyed after clean start
        {
            // To ensure following steps are performed after the static init phase, they must be performed inside the setup()
            this->_devices.reset(new Devices());
            this->updateTimeProvider();
#if OSW_PLATFORM_ENVIRONMENT == 1
            this->_environment.reset(new Environment());
            this->environment()->updateProviders();
#endif
        }

        this->setupPower(fromLightSleep);
        this->setupButtons();
        this->setupFileSystem();
        this->setupDisplay(); // This also (re-)sets the brightness and enables the display
    } else {
        this->setupPower(fromLightSleep);
        this->displayOn();
    }

    this->devices()->setup(fromLightSleep);
    this->devices()->update(); // Update internal cache to refresh / initialize the value obtained by calling this->getAccelStepCount() - needed for e.g. the step statistics!
    this->updateTimezoneOffsets(); // Always update, just in case DST changed during (light) sleep - after all devices are setup/updated, as they might use their time for this calculation
#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1 && defined(OSW_FEATURE_STATS_STEPS)
    this->environment()->setupStepStatistics();
#endif

    randomSeed(this->getUTCTime()); // Make sure the RTC is loaded and get the real time (!= 0, differs from time(nullptr), which is possibly 0 after deep sleep)
    OswServiceManager::getInstance().setup(); // Fire off the service manager (this is here, as some services are loading their own hardware - unmanaged by us)
}

void OswHal::stop(bool toLightSleep) {
    // Do not stop the external sensor accelorometer, as it is maybe used for wakeup detection (and step counting)
    // Do not stop the external rtc, as it is maybe used for KEEPING THE TIME
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
    this->gpsBackupMode();
    this->sdOff();
#endif
    this->devices()->stop(toLightSleep);

    this->displayOff(); // This disables the display
    OswServiceManager::getInstance().stop();

    if (!toLightSleep) {
        this->gfx()->fillBuffer(rgb565(0,0,0));  // This makes the display black
        this->flushCanvas();
#if OSW_PLATFORM_ENVIRONMENT == 1
        this->_environment.reset();
#endif
        this->_devices.reset();
        this->timeProvider = nullptr; // He is properly destroyed after devices destruction ↑
    }
    OSW_LOG_D(toLightSleep ? "-> light sleep " : "-> deep sleep ");
}

#if OSW_PLATFORM_IS_FLOW3R_BADGE == 1
uint8_t OswHal::readGpioExtender(uint8_t address) {
    Wire.beginTransmission(address);
    Wire.write(0xFF); // we do not want to output anything (who  knows if this is a good idea)
    uint8_t error = Wire.endTransmission();
    if (error != 0)
        OSW_LOG_W("Failed to communicate with GPIO extender chip!");
    uint8_t bytes = 1; // using a variable to prevent compiler warnings..
    Wire.requestFrom(address, bytes);
    return Wire.read();
}
#endif
