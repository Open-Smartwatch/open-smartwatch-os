#include "./services/OswServiceManager.h"

#include "./services/OswServiceTasks.h"
#include "esp_task_wdt.h"

/**
 * This enables all currently known services using their setup() and starts the loop() on core 0
 */
void OswServiceManager::setup() {
    for (unsigned char i = 0; i < oswServiceTasksCount; i++)
        if(oswServiceTasks[i])
            oswServiceTasks[i]->setup();
    this->active = true;
    #ifndef OSW_EMULATOR
    xTaskCreatePinnedToCore([](void *pvParameters) -> void { OswServiceManager::getInstance().worker(); },
                            "oswServiceManager", this->workerStackSize /*stack*/, NULL /*input*/, 0 /*prio*/,
                            &this->core0worker /*handle*/, 0);
    #else
    this->core0worker = new std::thread([]() -> void { OswServiceManager::getInstance().worker(); });
    #endif
}

/**
 * Waits 2000ms and then starts the task loop
 */
void OswServiceManager::worker() {
    delay(2000);  // Wait two seconds to give the rest of the OS time to boot (in case a service causes a system crash -
    // wifi)
#ifndef NDEBUG
    Serial.println(String(__FILE__) + ": Background worker started.");
#endif
    while (this->active) {
        this->loop();
        delay(10);  // Give the kernel time to do his stuff (as we are normally running this on his core 0)
    }
#ifndef NDEBUG
    Serial.println(String(__FILE__) + ": Background worker terminated!");
#endif
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
    for (unsigned char i = 0; i < oswServiceTasksCount; i++)
        if(oswServiceTasks[i])
            oswServiceTasks[i]->stop();
    this->active = false;
    #ifdef OSW_EMULATOR
    if(this->core0worker and this->core0worker->joinable()) {
        this->core0worker->join();
        delete this->core0worker;
        this->core0worker = nullptr;
    }
    #endif
}