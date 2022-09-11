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
        static OswServiceManager instance;
        return instance;
    }

    //Temp workaround until #137 is done
#ifdef OSW_FEATURE_WIFI
#define _SERVICE_WIFI 1
#else
#define _SERVICE_WIFI 0
#endif
    const unsigned workerStackSize = 1024 + (7168 * _SERVICE_WIFI); // If wifi is active, set to same size as core 0

    void setup();
    void loop();
    void stop();

  private:
    OswServiceManager() {};
    void worker();
    #ifndef OSW_EMULATOR
    TaskHandle_t core0worker;
    #else
    std::thread* core0worker;
    #endif
    bool active = false;

    OswServiceManager(OswServiceManager const&);
    void operator=(OswServiceManager const&);
};
#endif
