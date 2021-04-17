#ifndef OSW_SERVICE_MANAGER_H
#define OSW_SERVICE_MANAGER_H
#include "osw_service.h"
#include "osw_hal.h"
#include <vector>


class OswServiceManager {
    public:
        static OswServiceManager &getInstance() {
            static OswServiceManager instance;
            return instance;
        }

        void registerService(int serviceId, OswService *service);
        OswService *getService(int serviceId);

        void setup(OswHal *hal);
        void loop(OswHal *hal);
        void stop(OswHal *hal);

    private:
        OswServiceManager(): serviceRegistry() {};

        OswServiceManager(OswServiceManager const&);
        void operator=(OswServiceManager const&);

        struct Service {
            int id;
            OswService *service;
        };
        
        std::vector<Service> serviceRegistry; 
};
#endif