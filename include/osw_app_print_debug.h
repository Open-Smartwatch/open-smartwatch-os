#ifndef OSW_APP_PRINT_DEBUG_H
#define OSW_APP_PRINT_DEBUG_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppPrintDebug : public OswApp {
 public:
  OswAppPrintDebug(void){};
  void loop(OswHal* hal);
  ~OswAppPrintDebug(){};

 private:
};

#endif