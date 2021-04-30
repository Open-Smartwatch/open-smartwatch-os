#ifndef OSW_DEFAULTLAUNCHER_H
#define OSW_DEFAULTLAUNCHER_H

#include <osw_hal.h>
#include <osw_launcher.h>


class OswDefaultLauncher : public OswLauncher {
 public:
  OswDefaultLauncher(void){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswDefaultLauncher(){};

 private:
 
};

#endif