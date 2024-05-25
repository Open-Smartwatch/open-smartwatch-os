#ifndef OSW_EMULATOR
#include "./services/OswServiceTaskMemMonitor.h"

#include "osw_hal.h"
#include "osw_ui.h"
#include "services/OswServiceManager.h"
#include "services/OswServiceTasks.h"
#ifdef OSW_FEATURE_WIFI
#include "services/OswServiceTaskWebserver.h"
#endif
#include "services/OswServiceTaskBLECompanion.h"

void OswServiceTaskMemMonitor::setup() {
    OswServiceTask::setup();
}

/**
 * Updates the current high water mark for the core on which this service is running on (core 0),
 * updates the high watermark for the heap and calls printStats() on changes
 */
void OswServiceTaskMemMonitor::loop() {
    unsigned core0 = uxTaskGetStackHighWaterMark(nullptr);
    unsigned high = xPortGetMinimumEverFreeHeapSize();
    if (core0 != this->core0high or this->heapHigh != high) {
        this->core0high = core0;
        this->heapHigh = high;
        this->printStats();
    }

    // Check for a low memory condition and try to free system resources to help out
    bool nowLowMemoryCondition = false;
#ifdef OSW_FEATURE_WIFI
    if(OswServiceAllTasks::webserver.webserverActive()) {
        if(this->lowMemoryCondition or ESP.getFreeHeap() < 100000) // Keep low memory or enable if less than 100kb available
            nowLowMemoryCondition = true;
    }
#endif
#if SERVICE_BLE_COMPANION == 1
    if(OswServiceAllTasks::bleCompanion.isRunning()) {
        if(this->lowMemoryCondition or ESP.getFreeHeap() < 400000) // Keep low memory or enable if less than 400kb available
            nowLowMemoryCondition = true;
    }
#endif

    if(nowLowMemoryCondition and !this->lowMemoryCondition)
        OSW_LOG_W("Low memory condition! Activating countermeasures...");
    if(!nowLowMemoryCondition and this->lowMemoryCondition)
        OSW_LOG_I("Low memory condition resolved. Deactivating countermeasures...");

    if(this->lowMemoryCondition != nowLowMemoryCondition) {
        OswUI* ui = OswUI::getInstance();
        std::lock_guard<std::mutex> noRender(*ui->drawLock);
        if(nowLowMemoryCondition) {
            OswHal::getInstance()->disableDisplayBuffer();
            OSW_LOG_I("Disabled display buffering.");
        } else {
            OswHal::getInstance()->enableDisplayBuffer();
            OSW_LOG_I("Enabled display buffering.");
        }
    }

    this->lowMemoryCondition = nowLowMemoryCondition;
}

bool OswServiceTaskMemMonitor::hasLowMemoryCondition() {
    return this->lowMemoryCondition;
}

/**
 * This should periodically be called from the loop on core 1, it updates his stack high
 * watermarks and also calls printStats() on changes
 */
void OswServiceTaskMemMonitor::updateLoopTaskStats() {
    unsigned core1 = uxTaskGetStackHighWaterMark(nullptr);
    if (core1 != this->core1high) {
        this->core1high = core1;
        this->printStats();
    }
}

/**
 * Send a overview regarding the current stack watermarks (core 0&1), heap watermarks and heap ussage to serial
 */
void OswServiceTaskMemMonitor::printStats() {
    String msg = "========= Memory Monitor =========\n";
    msg += "core 0 (high):\t";
    msg += OswServiceManager::getInstance().workerStackSize - this->core0high;
    msg += "B of ";
    msg += OswServiceManager::getInstance().workerStackSize;
    msg += "B\n";

    const unsigned maxCore1 =
        8192;  // This value is based on https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/main.cpp
    msg += "core 1 (high):\t";
    msg += maxCore1 - this->core1high;
    msg += "B of ";
    msg += maxCore1;
    msg += "B\n";

    msg += "heap (high):\t";
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
    msg += (ESP.getHeapSize() + ESP.getPsramSize()) - this->heapHigh;
#else
    msg += ESP.getHeapSize() - this->heapHigh;
#endif
    msg += "B of ";
    msg += ESP.getHeapSize();
    msg += "B\n";

    msg += "heap (curr):\t";
    msg += ESP.getHeapSize() - ESP.getFreeHeap();
    msg += "B of ";
    msg += ESP.getHeapSize();
    msg += "B\n";

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
    msg += "psram (curr):\t";
    msg += ESP.getPsramSize() - ESP.getFreePsram();
    msg += "B of ";
    msg += ESP.getPsramSize();
    msg += "B\n";
#endif

    // TODO Maybe fetch current largest available heap size and calc "fragmentation" percentage.
    msg.trim();
    OSW_LOG_D(msg);
}
#endif
