#include "osw_service.h"

class OswServiceTaskBLECompanion;
class OswServiceTaskBLEServer;
class OswServiceTaskExample;
class OswServiceTaskMemMonitor;
class OswServiceTaskNotifier;
#ifdef OSW_FEATURE_WIFI
class OswServiceTaskWiFi;
class OswServiceTaskWebserver;
#endif
namespace OswServiceAllTasks {
#if SERVICE_BLE_COMPANION == 1
extern OswServiceTaskBLECompanion bleCompanion;
#endif
// extern OswServiceTaskExample example;
#ifdef OSW_FEATURE_WIFI
extern OswServiceTaskWiFi wifi;
extern OswServiceTaskWebserver webserver;
#endif
#if OSW_SERVICE_NOTIFIER == 1
extern OswServiceTaskNotifier notifier;
#endif
#ifdef OSW_FEATURE_BLE_SERVER
extern OswServiceTaskBLEServer bleServer;
#endif
extern OswServiceTaskMemMonitor memory;
}

extern const unsigned char oswServiceTasksCount;
extern OswServiceTask* oswServiceTasks[];
