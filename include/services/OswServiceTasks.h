#include "osw_service.h"

class OswServiceTaskBLECompanion;
class OswServiceTaskExample;
class OswServiceTaskMemMonitor;
#ifdef OSW_FEATURE_WIFI
class OswServiceTaskWiFi;
class OswServiceTaskWebserver;
#endif
namespace OswServiceAllTasks {
#if SERVICE_BLE_COMPANION == 1
extern OswServiceTaskBLECompanion bleCompanion;
#endif
//extern OswServiceTaskExample example;
#ifdef OSW_FEATURE_WIFI
extern OswServiceTaskWiFi wifi;
extern OswServiceTaskWebserver webserver;
#endif
extern OswServiceTaskMemMonitor memory;
}

extern const unsigned char oswServiceTasksCount;
extern OswServiceTask* oswServiceTasks[];
