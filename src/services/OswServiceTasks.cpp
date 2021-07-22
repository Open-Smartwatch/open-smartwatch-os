#include "services/OswServiceTasks.h"

#include "services/OswServiceTaskBLECompanion.h"
#include "services/OswServiceTaskExample.h"
#include "services/OswServiceTaskGPS.h"
#include "services/OswServiceTaskMemMonitor.h"
#if SERVICE_WIFI == 1
#include "services/OswServiceTaskWiFi.h"
#endif
#include "osw_util.h"
#include "services/OswServiceTaskWebserver.h"

namespace OswServiceAllTasks {
// OswServiceTaskExample example;
#if SERVICE_BLE_COMPANION == 1
OswServiceTaskBLECompanion bleCompanion;
#endif
#ifdef GPS_EDITION
OswServiceTaskGPS gps;
#endif
#if SERVICE_WIFI == 1
OswServiceTaskWiFi wifi;
OswServiceTaskWebserver webserver;
#endif
#ifdef DEBUG
OswServiceTaskMemMonitor memory;
#endif
}  // namespace OswServiceAllTasks

OswServiceTask* oswServiceTasks[] = {
#if SERVICE_BLE_COMPANION == 1
    &OswServiceAllTasks::bleCompanion,
#endif
#ifdef GPS_EDITION
    &OswServiceAllTasks::gps,
#endif
//&OswServiceAllTasks::example,
#if SERVICE_WIFI == 1
    &OswServiceAllTasks::wifi, &OswServiceAllTasks::webserver,
#endif
#ifdef DEBUG
    &OswServiceAllTasks::memory
#endif
};
const unsigned char oswServiceTasksCount = OswUtil::size(oswServiceTasks);
