#pragma once

#include <osw_ui.h>
#include <osw_hal.h>
#include "osw_app.h"

class OswAppCalendar : public OswApp {
  public:
    OswAppCalendar(void) {};
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppCalendar() {};

  private:
    uint8_t y = 90;
    uint8_t x = 52;
    void printEvent(const char *date, const char *event, uint16_t color);
};