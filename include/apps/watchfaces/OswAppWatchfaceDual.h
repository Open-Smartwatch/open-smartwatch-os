#pragma once

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"

class OswAppWatchfaceDual : public OswApp {
  public:
    OswAppWatchfaceDual(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppWatchfaceDual() {};

    //for dual-time
    static void drawProgressBar(OswUI* ui,uint8_t cx, uint8_t cy, uint8_t jump, uint8_t length, uint8_t value,float angle, uint8_t radius, uint16_t color, uint8_t* goal=nullptr);
    void drawAnimSec();

  private:
    OswUI* ui;
    int buttonMode = 1;

    uint8_t pos;
};