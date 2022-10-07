#pragma once

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"

class OswAppWatchfaceMonotimer : public OswApp {
  public:
    OswAppWatchfaceMonotimer(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppWatchfaceMonotimer() {};
    static void drawHour();

  private:
    OswUI* ui;
    void drawWatch();
    void drawNShiftedTicks(Graphics2D* gfx, uint8_t cx, uint8_t cy, uint8_t r1, uint8_t r2, uint8_t nTicks, float shift, uint16_t color);
    void drawNShiftedMaskedTicks(Graphics2D* gfx, uint8_t cx, uint8_t cy, uint8_t r1, uint8_t r2, uint8_t nTicks, float shift, uint16_t m, uint16_t color);
};
