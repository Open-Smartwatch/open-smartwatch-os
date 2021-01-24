#ifndef OSW_APP_FIREWORKS_H
#define OSW_APP_FIREWORKS_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppFireworks : public OswApp {
 public:
  OswAppFireworks(void){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  ~OswAppFireworks(){};

 private:
};

#endif