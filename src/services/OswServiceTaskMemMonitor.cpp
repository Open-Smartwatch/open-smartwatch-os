#include "./services/OswServiceTaskMemMonitor.h"

#include "osw_hal.h"
#include "services/OswServiceManager.h"

void OswServiceTaskMemMonitor::setup(OswHal* hal) { OswServiceTask::setup(hal); }

void OswServiceTaskMemMonitor::loop(OswHal* hal) {
  unsigned core0 = uxTaskGetStackHighWaterMark(nullptr);
  unsigned high = xPortGetMinimumEverFreeHeapSize();
  if (core0 != this->core0high or this->heapHigh != high) {
    this->core0high = core0;
    this->heapHigh = high;
    this->printStats();
  }
}

void OswServiceTaskMemMonitor::updateLoopTaskStats() {
  unsigned core1 = uxTaskGetStackHighWaterMark(nullptr);
  if (core1 != this->core1high) {
    this->core1high = core1;
    this->printStats();
  }
}

void OswServiceTaskMemMonitor::printStats() {
  Serial.println("========= Memory Monitor =========");
  Serial.print("core 0 (high):\t");
  Serial.print(OswServiceManager::getInstance().workerStackSize - this->core0high);
  Serial.print("B of ");
  Serial.print(OswServiceManager::getInstance().workerStackSize);
  Serial.println("B");

  const unsigned maxCore1 = 8192; // This value is based on https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/main.cpp
  Serial.print("core 1 (high):\t");
  Serial.print(maxCore1 - this->core1high);
  Serial.print("B of ");
  Serial.print(maxCore1);
  Serial.println("B");

  Serial.print("heap (high):\t");
  Serial.print(ESP.getHeapSize() - this->heapHigh);
  Serial.print("B of ");
  Serial.print(ESP.getHeapSize());
  Serial.println("B");

  Serial.print("heap (curr):\t");
  Serial.print(ESP.getHeapSize() - ESP.getFreeHeap());
  Serial.print("B of ");
  Serial.print(ESP.getHeapSize());
  Serial.println("B");
}