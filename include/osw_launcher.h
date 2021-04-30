#ifndef OSW_LAUNCHER_H
#define OSW_LAUNCHER_H

#include <osw_hal.h>
#include <osw_app.h>

class OswLauncher {
 public:
  virtual void setup(OswHal* hal) = 0;
  virtual void loop(OswHal* hal) = 0;
  virtual void stop(OswHal* hal) = 0;
 protected:
  OswApp *loadedApps[];
  uint8_t appCount;
  uint8_t currentAppIndex;
  void load(OswApp* app);
};

#endif