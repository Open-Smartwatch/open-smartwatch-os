#ifndef OSW_SERVICE_MANAGER_H
#define OSW_SERVICE_MANAGER_H
#include "osw_hal.h"
#include "osw_service.h"

#ifdef OSW_EMULATOR
#include <thread>
#endif

class OswServiceManager {
  public:
    static OswServiceManager& getInstance() {
        if(instance == nullptr) {
            instance.reset(new OswServiceManager());
        }
        return *instance.get();
    }
    static void resetInstance() {
        instance.reset();
    }

    // Temp workaround until #137 is done
#ifdef OSW_FEATURE_WIFI
#define _ADDITIONAL_STACK_SIZE_FOR_WIFI 8192
#else
#define _ADDITIONAL_STACK_SIZE_FOR_WIFI 0
#endif
#ifdef OSW_FEATURE_BLE_SERVER
#define _ADDITIONAL_STACK_SIZE_FOR_BLE 2048
#else
#define _ADDITIONAL_STACK_SIZE_FOR_BLE 0
#endif
    const unsigned workerStackSize = 2048 + _ADDITIONAL_STACK_SIZE_FOR_WIFI + _ADDITIONAL_STACK_SIZE_FOR_BLE; // base stack size is 2k
    const unsigned workerStartupDelay = 2000;
    const unsigned workerLoopDelay = 10;

    void setup();
    void loop();
    void stop();

  protected:
    ~OswServiceManager() {
        this->stop();
    };

    friend std::unique_ptr<OswServiceManager>::deleter_type;
  private:
    static std::unique_ptr<OswServiceManager> instance;
#ifndef OSW_EMULATOR
    TaskHandle_t core0worker;
#else
    std::unique_ptr<std::jthread> core0worker;
#endif
    bool active = false;

    OswServiceManager() {};
    void worker();
};
#endif
