#include "services/OswServiceTasks.h"

#include "services/OswServiceTaskBLECompanion.h"
#include "services/OswServiceTaskExample.h"
#include "services/OswServiceTaskGPS.h"
#include "services/OswServiceTaskMemMonitor.h"
#include "services/OswServiceTaskWiFi.h"
#include "services/OswServiceTaskWebserver.h"

#include "osw_util.h"

namespace OswServiceAllTasks {
#if SERVICE_BLE_COMPANION == 1
OswServiceTaskBLECompanion bleCompanion;
#endif
#ifdef GPS_EDITION
OswServiceTaskGPS gps;
#endif
//OswServiceTaskExample example;
OswServiceTaskWiFi wifi;
OswServiceTaskWebserver webserver;
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
    &OswServiceAllTasks::wifi,
    &OswServiceAllTasks::webserver,
#ifdef DEBUG
    &OswServiceAllTasks::memory
#endif
};
const unsigned char oswServiceTasksCount = OswUtil::size(oswServiceTasks);