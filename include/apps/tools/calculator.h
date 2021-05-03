#ifndef OSW_APP_CALCULATOR
#define OSW_APP_CALCULATOR_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppCalculator : public OswApp {
 public:
  OswAppCalculator(void){};
  void setup(OswHal * hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppCalculator(){};

 private:
};

#endif