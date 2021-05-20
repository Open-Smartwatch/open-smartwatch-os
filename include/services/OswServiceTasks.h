#include "osw_service.h"

class OswServiceTaskBLECompanion;

namespace OswServiceAllTasks {
#ifdef SERVICE_BLE_COMPANION
    extern OswServiceTaskBLECompanion bleCompanion;
#endif
}

extern const unsigned char oswServiceTasksCount;
extern OswServiceTask* oswServiceTasks[];