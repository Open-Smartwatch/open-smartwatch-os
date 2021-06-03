#include "services/OswServiceTasks.h"

#include "services/OswServiceTaskBLECompanion.h"
#include "services/OswServiceTaskExample.h"
#include "services/OswServiceTaskGPS.h"
#include "services/OswServiceTaskMemMonitor.h"
#include "services/OswServiceTaskWiFi.h"
#include "services/OswServiceTaskRawScreenServer.h"
#include "services/OswServiceTaskWebserver.h"

// Define helpers - as some are just defines and do not represent a numeric value (which is needed for the service amount calc)
#ifdef GPS_EDITION
    # define SERVICE_GPS 1
#else
    # define SERVICE_GPS 0
#endif
#ifdef DEBUG
    # define SERVICE_MEMMONITOR 1
#else
    # define SERVICE_MEMMONITOR 0
#endif
#ifdef RAW_SCREEN_SERVER
    #define SERVICE_RAW_SCREEN 1
#else
    #define SERVICE_RAW_SCREEN 0
#endif

namespace OswServiceAllTasks {
#if SERVICE_BLE_COMPANION == 1
OswServiceTaskBLECompanion bleCompanion;
#endif
#ifdef GPS_EDITION
OswServiceTaskGPS gps;
#endif
#if SERVICE_RAW_SCREEN == 1
OswServiceTaskRawScreenServer screenServer;
#endif
//OswServiceTaskExample example;
OswServiceTaskWiFi wifi;
OswServiceTaskWebserver webserver;
#ifdef DEBUG
OswServiceTaskMemMonitor memory;
#endif
}  // namespace OswServiceAllTasks

const unsigned char oswServiceTasksCount = 2 + SERVICE_BLE_COMPANION + SERVICE_GPS + SERVICE_MEMMONITOR + SERVICE_RAW_SCREEN;
OswServiceTask* oswServiceTasks[] = {
#if SERVICE_BLE_COMPANION == 1
    &OswServiceAllTasks::bleCompanion,
#endif
#ifdef GPS_EDITION
    &OswServiceAllTasks::gps,
#endif
    //&OswServiceAllTasks::example,
    &OswServiceAllTasks::wifi,
#if SERVICE_RAW_SCREEN == 1
    &OswServiceAllTasks::screenServer,
#endif
    &OswServiceAllTasks::webserver,
#ifdef DEBUG
    &OswServiceAllTasks::memory
#endif
};
