#include "osw_service.h"

class OswServiceTaskBLECompanion;
class OswServiceTaskExample;

namespace OswServiceAllTasks {
#if SERVICE_BLE_COMPANION == 1
    extern OswServiceTaskBLECompanion bleCompanion;
#endif
    extern OswServiceTaskExample example;
}

extern const unsigned char oswServiceTasksCount;
extern OswServiceTask* oswServiceTasks[];