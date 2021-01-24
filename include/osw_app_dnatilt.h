#ifndef OSW_APP_DNA_TILT_H
#define OSW_APP_DNA_TILT_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppDNATilt : public OswApp {
 public:
  OswAppDNATilt(void){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  ~OswAppDNATilt(){};

 private:
};

#endif