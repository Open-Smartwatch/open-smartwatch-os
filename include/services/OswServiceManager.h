#ifndef OSW_SERVICE_MANAGER_H
#define OSW_SERVICE_MANAGER_H
#include "osw_service.h"
#include "osw_hal.h"


class OswServiceManager {
    public:
        static OswServiceManager &getInstance() {
            static OswServiceManager instance;
            return instance;
        }
        const unsigned workerStackSize = 1024 + 1024 + 1024 + 1024; //Wifi service needs 1024, webserver another 1024 (the updater another 1024)

        void setup(OswHal *hal);
        void loop(OswHal *hal);
        void stop(OswHal *hal);
    private:
        OswServiceManager() {};
        void worker();
        TaskHandle_t core0worker;
        OswHal* workerHal = nullptr;
        bool active = false;

        OswServiceManager(OswServiceManager const&);
        void operator=(OswServiceManager const&);
};
#endif
