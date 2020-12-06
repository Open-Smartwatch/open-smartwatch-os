#ifndef OSW_APP_AUTUMN_H
#define OSW_APP_AUTUMN_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppAutumn : public OswApp {
 public:
  OswAppAutumn(){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  ~OswAppAutumn(){};

 private:
};

#endif