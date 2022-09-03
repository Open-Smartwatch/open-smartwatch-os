#pragma once

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"
#ifdef ANIMATION
#include <anim_matrix.h>
#endif

class OswAppWatchfaceMonotimer : public OswApp {
  public:
    OswAppWatchfaceMonotimer(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppWatchfaceMonotimer() {};

#ifdef OSW_FEATURE_STATS_STEPS
    static void drawStepHistory(OswUI* ui, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t max);
#endif
  private:
    OswUI* ui;
    void drawWatch();
	void drawNShiftedTicks(Graphics2D *gfx, uint8_t cx, uint8_t cy, uint8_t r1, uint8_t r2, uint8_t nTicks, float shift, uint16_t color);
	void drawNShiftedMaskedTicks(Graphics2D *gfx, uint8_t cx, uint8_t cy, uint8_t r1, uint8_t r2, uint8_t nTicks, float shift, uint16_t m, uint16_t color);
#ifdef ANIMATION
    AnimMatrix* matrix;
#endif
};
