#include <osw_hal.h>

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