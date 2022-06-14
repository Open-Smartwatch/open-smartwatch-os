#pragma once

#include <osw_hal.h>
#include "osw_app.h"

class OswAppPrintDebug : public OswApp {
  public:
    OswAppPrintDebug(void) {};
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppPrintDebug() {};

  private:
};