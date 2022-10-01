#pragma once

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"

class OswAppWatchfaceNumerals : public OswApp {
  public:
    OswAppWatchfaceNumerals(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppWatchfaceNumerals() {};

  private:
    OswUI* ui;
    void drawWatch();
};
