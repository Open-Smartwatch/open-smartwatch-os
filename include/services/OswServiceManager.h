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

        void setup(OswHal *hal);
        void loop(OswHal *hal);
        void stop(OswHal *hal);
    private:
        OswServiceManager() {};

        OswServiceManager(OswServiceManager const&);
        void operator=(OswServiceManager const&);
};
#endif