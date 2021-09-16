#ifndef OSW_APP_COMPASS_CALIBRATE_H
#define OSW_APP_COMPASS_CALIBRATE_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppMagnetometerCalibrate : public OswApp {
 public:
  OswAppMagnetometerCalibrate(void){};
  virtual void setup() override;
  virtual void loop() override;
  virtual void stop() override;
  ~OswAppMagnetometerCalibrate(){};

 private:
};

#endif
