#include "./services/servicemanager.h"

void OswServiceManager::registerService(int serviceId, OswService *service) {
    if (!getService(serviceId)) {
        Service svc = {serviceId, service};
        serviceRegistry.push_back(svc);
    }
}

void OswServiceManager::setup(OswHal *hal) {
    for (std::vector<Service>::iterator it = serviceRegistry.begin() ; it != serviceRegistry.end(); ++it) {
        return it->service->setup(hal);
    }
}

void OswServiceManager::loop(OswHal *hal) {
    for (std::vector<Service>::iterator it = serviceRegistry.begin() ; it != serviceRegistry.end(); ++it) {
        return it->service->loop(hal);
    }
}

void OswServiceManager::stop(OswHal *hal) {
    for (std::vector<Service>::iterator it = serviceRegistry.begin() ; it != serviceRegistry.end(); ++it) {
        return it->service->stop(hal);
    }
}

OswService *OswServiceManager::getService(int serviceId) {
    for (std::vector<Service>::iterator it = serviceRegistry.begin() ; it != serviceRegistry.end(); ++it) {
        if (it->id == serviceId) {
            return it->service;
        }
    }

    return NULL;
}