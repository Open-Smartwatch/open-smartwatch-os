#pragma once

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"

class OswAppWatchfaceMix : public OswApp {
  public:
    OswAppWatchfaceMix(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppWatchfaceMix() {};

    void dateDisplay();
    void digitalWatchDisplay();

  private:
    void analogWatchDisplay(); //thin
    int buttonMode = 1;
    OswUI* ui;
    uint8_t pos;
};
