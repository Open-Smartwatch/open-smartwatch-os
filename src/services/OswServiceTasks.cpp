#include "services/OswServiceTasks.h"
#include "services/OswServiceTaskBLECompanion.h"
#include "services/OswServiceTaskExample.h"

namespace OswServiceAllTasks {
#if SERVICE_BLE_COMPANION == 1
OswServiceTaskBLECompanion bleCompanion;
#endif
OswServiceTaskExample example;
}  // namespace OswServiceAllTasks

const unsigned char oswServiceTasksCount = 1 + SERVICE_BLE_COMPANION;
OswServiceTask* oswServiceTasks[] = {
#if SERVICE_BLE_COMPANION == 1
    &OswServiceAllTasks::bleCompanion,
#endif
    &OswServiceAllTasks::example};