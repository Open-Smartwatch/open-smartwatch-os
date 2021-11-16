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
  
  //Temp workaround until #137 is done
  #ifdef OSW_FEATURE_WIFI
  #define _SERVICE_WIFI 1
  #else
  #define _SERVICE_WIFI 0
  #endif
  const unsigned workerStackSize = 1024 + ((1024 + 1024 + 1024) * _SERVICE_WIFI);

  void setup();
  void loop();
  void stop();

 private:
  OswServiceManager(){};
  void worker();
  TaskHandle_t core0worker;
  bool active = false;

  OswServiceManager(OswServiceManager const &);
  void operator=(OswServiceManager const &);
};
#endif
