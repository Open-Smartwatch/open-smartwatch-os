#ifndef OSW_APP_FIREWORKS_H
#define OSW_APP_FIREWORKS_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppFireworks : public OswApp {
 public:
  OswAppFireworks(void){};
  virtual void setup() override;
  virtual void loop() override;
  ~OswAppFireworks(){};

 private:
};

#endif