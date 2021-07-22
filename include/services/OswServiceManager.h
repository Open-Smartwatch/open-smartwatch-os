#ifndef OSW_SERVICE_MANAGER_H
#define OSW_SERVICE_MANAGER_H
#include "osw_hal.h"
#include "osw_service.h"

class OswServiceManager {
 public:
  static OswServiceManager &getInstance() {
    static OswServiceManager instance;
    return instance;
  }
  // + wifi service needs 1024
  // + webserver another 1024
  // + updater another 1024
  const unsigned workerStackSize = 1024 + ((1024 + 1024 + 1024) * SERVICE_WIFI);

  void setup(OswHal *hal);
  void loop(OswHal *hal);
  void stop(OswHal *hal);

 private:
  OswServiceManager(){};
  void worker();
  TaskHandle_t core0worker;
  OswHal *workerHal = nullptr;
  bool active = false;

  OswServiceManager(OswServiceManager const &);
  void operator=(OswServiceManager const &);
};
#endif
