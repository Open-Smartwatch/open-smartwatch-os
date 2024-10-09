#pragma once
#include <osw_hal.h>

#ifdef OSW_EMULATOR
#include "imgui.h"
#endif

class OswAppV1 {
  public:
    virtual void setup() = 0;
    virtual void loop() = 0;
    virtual void stop() = 0;
#ifdef OSW_EMULATOR
    virtual void loopDebug() {}; // By default no debug loop (GUI) is implemented
#endif
};

typedef OswAppV1 OswApp; // For backwards compatibility
