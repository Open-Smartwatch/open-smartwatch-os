#include "osw_service.h"
#ifdef OSW_FEATURE_BLE
class OswServiceTaskBLECompanion;
#endif
class OswServiceTaskExample;
class OswServiceTaskMemMonitor;
#ifdef OSW_FEATURE_WIFI
class OswServiceTaskWiFi;
class OswServiceTaskWebserver;
#endif
namespace OswServiceAllTasks {
#ifdef OSW_FEATURE_BLE
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
