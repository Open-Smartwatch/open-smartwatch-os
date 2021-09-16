#ifdef OSW_FEATURE_WIFI
#ifndef OSW_APP_RUNTIME_TEST_H
#define OSW_APP_RUNTIME_TEST_H

#include <osw_hal.h>

#include "osw_app.h"

class MiniIotClient;
class OswAppRuntimeTest : public OswApp {
 public:
  OswAppRuntimeTest(void){};
  virtual void setup() override;
  virtual void loop() override;
  virtual void stop() override;
  ~OswAppRuntimeTest(){};

 private:
MiniIotClient* miniIot = nullptr;
};

#endif
#endif