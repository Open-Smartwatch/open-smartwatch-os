#ifndef OSW_APP_AUTUMN_H
#define OSW_APP_AUTUMN_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppAutumn : public OswApp {
 public:
  OswAppAutumn(){};
  virtual void setup() override;
  virtual void loop() override;
  ~OswAppAutumn(){};

 private:
};

#endif