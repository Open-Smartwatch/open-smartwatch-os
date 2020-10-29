#ifndef OSW_APP_HELLO_WORLD_H
#define OSW_APP_HELLO_WORLD_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppHelloWorld : public OswApp {
 public:
  OswAppHelloWorld(void){};
  void run(OswHal* hal);
  ~OswAppHelloWorld(){};

 private:
};

#endif