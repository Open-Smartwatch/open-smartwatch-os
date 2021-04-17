#ifndef OSW_APP_MAP_H
#define OSW_APP_MAP_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppMap : public OswApp {
 public:
  OswAppMap(){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppMap(){};

 private:
};

#endif