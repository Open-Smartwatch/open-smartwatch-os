#ifndef OSW_APP_COMPASS_CALIBRATE_H
#define OSW_APP_COMPASS_CALIBRATE_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppMagnetometerCalibrate : public OswApp {
 public:
  OswAppMagnetometerCalibrate(void){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppMagnetometerCalibrate(){};

 private:
};

#endif
