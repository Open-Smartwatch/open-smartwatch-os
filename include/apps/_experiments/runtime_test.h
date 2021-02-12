#ifndef OSW_APP_RUNTIME_TEST_H
#define OSW_APP_RUNTIME_TEST_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppRuntimeTest : public OswApp {
 public:
  OswAppRuntimeTest(void){};
  void setup(OswHal * hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppRuntimeTest(){};

 private:
};

#endif