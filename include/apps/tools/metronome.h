#ifndef OSW_APP_METRONOME_H
#define OSW_APP_METRONOME_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppMetronome : public OswApp {
 public:
  OswAppMetronome(void){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppMetronome(){};

 private:
};

#endif
