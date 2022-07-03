#include "services/OswServiceTasks.h"

#include "services/OswServiceTaskBLECompanion.h"
#include "services/OswServiceTaskExample.h"
#include "services/OswServiceTaskGPS.h"
#include "services/OswServiceTaskMemMonitor.h"
#ifdef OSW_FEATURE_WIFI
#include "services/OswServiceTaskWiFi.h"
#include "services/OswServiceTaskWebserver.h"
#endif
#include "osw_util.h"

namespace OswServiceAllTasks {
// OswServiceTaskExample example;
#if SERVICE_BLE_COMPANION == 1
OswServiceTaskBLECompanion bleCompanion;
#endif
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)

OswServiceTaskGPS gps;
#endif
#ifdef OSW_FEATURE_WIFI
OswServiceTaskWiFi wifi;
OswServiceTaskWebserver webserver;
#endif
OswServiceTaskMemMonitor memory;
}  // namespace OswServiceAllTasks

OswServiceTask* oswServiceTasks[] = {
#if SERVICE_BLE_COMPANION == 1
    & OswServiceAllTasks::bleCompanion,
#endif
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)

    & OswServiceAllTasks::gps,
#endif
//&OswServiceAllTasks::example,
#ifdef OSW_FEATURE_WIFI
    & OswServiceAllTasks::wifi, &OswServiceAllTasks::webserver,
#endif
    & OswServiceAllTasks::memory
};
const unsigned char oswServiceTasksCount = OswUtil::size(oswServiceTasks);
