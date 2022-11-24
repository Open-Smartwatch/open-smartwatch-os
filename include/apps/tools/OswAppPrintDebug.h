#ifndef NDEBUG
#pragma once

#include <osw_hal.h>
#include "osw_app.h"

class OswAppPrintDebug : public OswApp {
  public:
    OswAppPrintDebug(void) {};
    virtual void setup() override;
    virtual void loop() override;
#ifdef OSW_EMULATOR
    virtual void loopDebug() override;
#endif
    virtual void stop() override;
    ~OswAppPrintDebug() {};

  private:
    uint8_t y = 32;
    uint8_t x = 52;
    void printStatus(const char* setting, const char* value);
};
#endif