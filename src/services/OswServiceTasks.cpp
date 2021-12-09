#include "services/OswServiceTasks.h"

#include "services/OswServiceTaskBLECompanion.h"
#include "services/OswServiceTaskExample.h"
#include "services/OswServiceTaskGPS.h"
#include "services/OswServiceTaskMemMonitor.h"
#ifdef OSW_FEATURE_WIFI
#include "services/OswServiceTaskWiFi.h"
#endif
#include "osw_util.h"
#include "services/OswServiceTaskWebserver.h"

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
#ifndef NDEBUG
OswServiceTaskMemMonitor memory;
#endif
}  // namespace OswServiceAllTasks

OswServiceTask* oswServiceTasks[] = {
#if SERVICE_BLE_COMPANION == 1
    &OswServiceAllTasks::bleCompanion,
#endif
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)

    &OswServiceAllTasks::gps,
#endif
//&OswServiceAllTasks::example,
#ifdef OSW_FEATURE_WIFI
    &OswServiceAllTasks::wifi, &OswServiceAllTasks::webserver,
#endif
#ifndef NDEBUG
    &OswServiceAllTasks::memory
#endif
};
const unsigned char oswServiceTasksCount = OswUtil::size(oswServiceTasks);
