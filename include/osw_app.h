#ifndef OSW_APP_H
#define OSW_APP_H

#include <osw_hal.h>

#ifdef OSW_EMULATOR
#include "imgui.h"
#endif

class OswApp {
  public:
    virtual void setup() = 0;
    virtual void loop() = 0;
    virtual void stop() = 0;
#ifdef OSW_EMULATOR
    virtual void loopDebug() {}; // By default no debug loop (GUI) is implemented
#endif
};

#endif