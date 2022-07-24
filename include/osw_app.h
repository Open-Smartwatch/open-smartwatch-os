#pragma once
#include <osw_hal.h>
class OswApp {
  public:
    virtual void setup() = 0;
    virtual void loop() = 0;
    virtual void stop() = 0;
};