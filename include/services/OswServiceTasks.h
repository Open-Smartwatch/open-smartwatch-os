#include "osw_service.h"

class OswServiceTaskBLECompanion;
class OswServiceTaskExample;
class OswServiceTaskMemMonitor;
class OswServiceTaskWiFi;

namespace OswServiceAllTasks {
#if SERVICE_BLE_COMPANION == 1
    extern OswServiceTaskBLECompanion bleCompanion;
#endif
    //extern OswServiceTaskExample example;
    extern OswServiceTaskWiFi wifi;
#ifdef DEBUG
    extern OswServiceTaskMemMonitor memory;
#endif
}

extern const unsigned char oswServiceTasksCount;
extern OswServiceTask* oswServiceTasks[];