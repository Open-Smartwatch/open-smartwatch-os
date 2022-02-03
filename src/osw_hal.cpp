#include <osw_hal.h>
#include <services/OswServiceManager.h>

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
#include "hal/esp32/sd_filesystem.h"
#else
#include "hal/esp32/spiffs_filesystem.h"
#endif

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
OswHal* OswHal::instance = new OswHal(new SDFileSystemHal());
#else
OswHal* OswHal::instance = new OswHal(new SPIFFSFileSystemHal());
#endif

OswHal* OswHal::getInstance() { return OswHal::instance; };

OswHal::OswHal(FileSystemHal* fs) : fileSystem(fs) {
    //begin I2c communication
    Wire.begin(OSW_DEVICE_I2C_SDA, OSW_DEVICE_I2C_SCL, 100000L);
}

OswHal::~OswHal() {

};

void OswHal::setup(bool fromLightSleep) {
    if(!fromLightSleep) {
        {
            // To ensure following steps are performed after the static init phase, they must be perfromed inside the setup()
            this->devices = new Devices();
            this->updateTimeProvider();
            #if OSW_PLATFORM_ENVIRONMENT == 1
            this->environment = new Environment();
            this->environment->updateProviders();
            #endif
        }
        this->setupPower();
        this->setupButtons();
        this->setupFileSystem();
        this->setupDisplay(); // This also (re-)sets the brightness and enables the display
    } else
        this->displayOn();
    this->devices->setup(fromLightSleep);
    this->devices->update(); // Update internal cache to refresh / initialize the value obtained by calling this->getAccelStepCount() - needed for e.g. the step statistics!
    #if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
    this->environment->setupStepStatistics();
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
    this->devices->stop(toLightSleep);

    this->displayOff(); // This disables the display
    OswServiceManager::getInstance().stop();

    if(!toLightSleep) {
        delete this->environment;
        delete this->devices;
    }
#ifndef NDEBUG
    Serial.println(toLightSleep ? "-> light sleep " : "-> deep sleep ");
#endif
    delay(100); // Make sure the Serial is flushed and any tasks are finished...
}