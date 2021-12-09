#include "osw_service.h"

class OswServiceTaskBLECompanion;
class OswServiceTaskExample;
class OswServiceTaskMemMonitor;
class OswServiceTaskWiFi;
class OswServiceTaskRawScreenServer;
class OswServiceTaskWebserver;

namespace OswServiceAllTasks {
#if SERVICE_BLE_COMPANION == 1
    extern OswServiceTaskBLECompanion bleCompanion;
#endif
    //extern OswServiceTaskExample example;
    extern OswServiceTaskWiFi wifi;
    extern OswServiceTaskWebserver webserver;
#ifndef NDEBUG
    extern OswServiceTaskMemMonitor memory;
#endif
#if SERVICE_RAW_SCREEN == 1
    extern OswServiceTaskRawScreenServer screenServer;
#endif
}

extern const unsigned char oswServiceTasksCount;
extern OswServiceTask* oswServiceTasks[];
