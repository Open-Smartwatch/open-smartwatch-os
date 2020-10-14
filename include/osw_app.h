#ifndef OSW_APP_H
#define OSW_APP_H

#include <osw_hal.h>

class OswApp {
 public:
  OswApp(void) {};
  virtual void run(OswHal* hal){};
  ~OswApp() {};
};

#endif