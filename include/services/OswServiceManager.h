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

    //Temp workaround until #137 is done
#ifdef OSW_FEATURE_WIFI
#define _SERVICE_WIFI 1
#else
#define _SERVICE_WIFI 0
#endif
    const unsigned workerStackSize = 2048 + (8192 * _SERVICE_WIFI); // If wifi is active, set to same size as core 0
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
