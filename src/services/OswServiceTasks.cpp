#include "services/OswServiceTasks.h"
#include "services/OswServiceTaskBLECompanion.h"

namespace OswServiceAllTasks {
#ifdef SERVICE_BLE_COMPANION
    OswServiceTaskBLECompanion bleCompanion;
#endif
}

#ifdef SERVICE_BLE_COMPANION
const unsigned char oswServiceTasksCount = 1;
#else
const unsigned char oswServiceTasksCount = 0;
#endif

OswServiceTask* oswServiceTasks[] = {
#ifdef SERVICE_BLE_COMPANION
    &OswServiceAllTasks::bleCompanion
#endif
};