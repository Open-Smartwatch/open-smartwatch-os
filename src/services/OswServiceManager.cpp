#include "./services/OswServiceManager.h"

#include "./services/OswServiceTasks.h"
#include "esp_task_wdt.h"

std::unique_ptr<OswServiceManager> OswServiceManager::instance = nullptr;

/**
 * This enables all currently known services using their setup() and starts the loop() on core 0
 */
void OswServiceManager::setup() {
    if (this->active) return;
    this->active = true;
    for (unsigned char i = 0; i < oswServiceTasksCount; i++)
        if(oswServiceTasks[i])
            oswServiceTasks[i]->setup();
#ifndef OSW_EMULATOR
    xTaskCreatePinnedToCore([](void* pvParameters) -> void { OswServiceManager::getInstance().worker(); },
                            "oswServiceManager", this->workerStackSize /*stack*/, NULL /*input*/, 0 /*prio*/,
                            &this->core0worker /*handle*/, 0);
#else
    this->core0worker.reset(new std::jthread([]() -> void { OswServiceManager::getInstance().worker(); }));
#endif
}

/**
 * Waits this->workerStartupDelay and then starts the task loop
 */
void OswServiceManager::worker() {
    delay(this->workerStartupDelay);  // Wait two seconds to give the rest of the OS time to boot (in case a service causes a system crash -
    // wifi)
    OSW_LOG_D("Background worker started.");
    while (this->active) {
        this->loop();
        delay(this->workerLoopDelay);  // Give the kernel time to do his stuff (as we are normally running this on his core 0)
    }
    OSW_LOG_D("Background worker terminated!");
#ifndef OSW_EMULATOR
    vTaskDelete(nullptr); // Inform FreeRTOS this task is done - otherwise the kernel will take that personally and crash!
#endif
}

void OswServiceManager::loop() {
    for (unsigned char i = 0; i < oswServiceTasksCount; i++)
        if (oswServiceTasks[i])
            if (oswServiceTasks[i]->isRunning())
                oswServiceTasks[i]->loop();
}

void OswServiceManager::stop() {
    if(!this->active) return;
    this->active = false;
    for (unsigned char i = 0; i < oswServiceTasksCount; i++)
        if(oswServiceTasks[i])
            oswServiceTasks[i]->stop();
}
