#ifndef OSW_APP_H
#define OSW_APP_H

#include <osw_hal.h>

class OswApp {
 public:
  OswApp(void){};
  void setup(OswHal* hal){};
  virtual void loop(OswHal* hal){};
  ~OswApp(){};
};

#endif