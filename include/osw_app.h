#ifndef OSW_APP_H
#define OSW_APP_H

#include <osw_hal.h>

class OswApp {
 public:
  virtual void setup() = 0;
  virtual void loop() = 0;
  virtual void stop() = 0;
};

#endif