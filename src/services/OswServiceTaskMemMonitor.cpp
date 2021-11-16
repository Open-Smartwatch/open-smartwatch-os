#include "./services/OswServiceTaskMemMonitor.h"

#include "osw_hal.h"
#include "services/OswServiceManager.h"

void OswServiceTaskMemMonitor::setup() { OswServiceTask::setup(); }

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
 * Send a overview regarding the current stack watermarks (core 0&1), heap watermarks and heap useage to serial
 */
void OswServiceTaskMemMonitor::printStats() {
  Serial.println("========= Memory Monitor =========");
  Serial.print("core 0 (high):\t");
  Serial.print(OswServiceManager::getInstance().workerStackSize - this->core0high);
  Serial.print("B of ");
  Serial.print(OswServiceManager::getInstance().workerStackSize);
  Serial.println("B");

  const unsigned maxCore1 =
      8192;  // This value is based on https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/main.cpp
  Serial.print("core 1 (high):\t");
  Serial.print(maxCore1 - this->core1high);
  Serial.print("B of ");
  Serial.print(maxCore1);
  Serial.println("B");

  Serial.print("heap (high):\t");
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
  Serial.print((ESP.getHeapSize() + ESP.getPsramSize()) - this->heapHigh);
#else
  Serial.print(ESP.getHeapSize() - this->heapHigh);
#endif
  Serial.print("B of ");
  Serial.print(ESP.getHeapSize());
  Serial.println("B");

  Serial.print("heap (curr):\t");
  Serial.print(ESP.getHeapSize() - ESP.getFreeHeap());
  Serial.print("B of ");
  Serial.print(ESP.getHeapSize());
  Serial.println("B");

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
  Serial.print("psram (curr):\t");
  Serial.print(ESP.getPsramSize() - ESP.getFreePsram());
  Serial.print("B of ");
  Serial.print(ESP.getPsramSize());
  Serial.println("B");
#endif

  // TODO Maybe fetch current largest available heap size and calc "fragmentation" percentage.
}
