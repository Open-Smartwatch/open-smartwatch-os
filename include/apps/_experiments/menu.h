#ifndef OSW_APP_MENU_H
#define OSW_APP_MENU_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppMenu : public OswApp {
 public:
  OswAppMenu(void){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppMenu(){};

 private:
 void printEntry(OswHal* hal, char** entries, uint8_t index, int8_t EntryCount);
 int8_t index;
 bool selectionInProgress;
 
};

#endif