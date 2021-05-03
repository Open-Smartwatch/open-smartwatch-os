#ifndef OSW_APP_H
#define OSW_APP_H

#include <osw_hal.h>

class OswApp {
  public:
    const char name[20] = "OswApp";
    virtual void setup(OswHal* hal) = 0;
    virtual void loop(OswHal* hal) = 0;
    virtual void stop(OswHal* hal) = 0;
};

#endif