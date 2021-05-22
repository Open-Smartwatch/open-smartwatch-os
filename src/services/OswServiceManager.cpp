#include "./services/OswServiceManager.h"

#include "./services/OswServiceTasks.h"
#include "esp_task_wdt.h"

/**
 * This enables all currently known services using their setup() and starts the loop() on core 0
 */
void OswServiceManager::setup(OswHal *hal) {
  for (unsigned char i = 0; i < oswServiceTasksCount; i++) oswServiceTasks[i]->setup(hal);
  this->active = true;
  this->workerHal = hal;
  xTaskCreatePinnedToCore([](void *pvParameters) -> void { OswServiceManager::getInstance().worker(); },
                          "oswServiceManager", this->workerStackSize /*stack*/, NULL /*input*/, 0 /*prio*/,
                          &this->core0worker /*handle*/, 0);
}

/**
 * Waits 2000ms and then starts the task loop
 */
void OswServiceManager::worker() {
  delay(2000);  // Wait two seconds to give the rest of the OS time to boot (in case a service causes a system crash -
                // wifi)
#ifdef DEBUG
  Serial.println(String(__FILE__) + ": Background worker started.");
#endif
  while (this->active) {
    this->loop(this->workerHal);
    delay(10);  // Give the kernel time to do his stuff (as we are normally running this on his core 0)
  }
#ifdef DEBUG
  Serial.println(String(__FILE__) + ": Background worker terminated!");
#endif
}

void OswServiceManager::loop(OswHal *hal) {
  for (unsigned char i = 0; i < oswServiceTasksCount; i++)
    if (oswServiceTasks[i]->isRunning()) oswServiceTasks[i]->loop(hal);
}

void OswServiceManager::stop(OswHal *hal) {
  for (unsigned char i = 0; i < oswServiceTasksCount; i++) oswServiceTasks[i]->stop(hal);
  this->active = false;
}