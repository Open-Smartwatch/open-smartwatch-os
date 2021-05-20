#include "osw_service.h"

class OswServiceTaskBLECompanion;
class OswServiceTaskExample;

namespace OswServiceAllTasks {
#ifdef SERVICE_BLE_COMPANION
    extern OswServiceTaskBLECompanion bleCompanion;
#endif
    extern OswServiceTaskExample example;
}

extern const unsigned char oswServiceTasksCount;
extern OswServiceTask* oswServiceTasks[];