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

void OswHal::setup(bool fromLightSleep) {
    if(!fromLightSleep) {
        this->setupPower();
        this->setupButtons();
        this->setupAccelerometer();
        this->setupTime();
        this->setupDisplay();
        this->setupFileSystem();
    }
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
    this->setupEnvironmentSensor();
#endif
    // The magnetometer is currently not setup/stopped by the hal. This should change.

    this->displayOn(); // This also (re-)sets the brightness
    randomSeed(this->getUTCTime()); // Make sure the RTC is loaded and get the real time (!= 0, differs from time(nullptr), which is possibly 0 after deep sleep)
    OswServiceManager::getInstance().setup(); // Fire off the service manager (this is here, as some services are loading their own hardware - unmanaged by us)
}

void OswHal::stop(bool toLightSleep) {
#ifdef DEBUG
    Serial.println(toLightSleep ? "-> light sleep " : "-> deep sleep ");
#endif

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
    this->gpsBackupMode();
    this->sdOff();
    this->stopEnvironmentSensor();
#endif

    this->displayOff();
    OswServiceManager::getInstance().stop();
}