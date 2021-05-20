#include "services/OswServiceTasks.h"
#include "services/OswServiceTaskBLECompanion.h"
#include "services/OswServiceTaskExample.h"

namespace OswServiceAllTasks {
#ifdef SERVICE_BLE_COMPANION
OswServiceTaskBLECompanion bleCompanion;
#endif
OswServiceTaskExample example;
}  // namespace OswServiceAllTasks

#ifndef SERVICE_BLE_COMPANION
const unsigned char oswServiceTasksCount = 1;
#else
const unsigned char oswServiceTasksCount = 2;
#endif

OswServiceTask* oswServiceTasks[] = {
#ifdef SERVICE_BLE_COMPANION
    &OswServiceAllTasks::bleCompanion,
#endif
    &OswServiceAllTasks::example};