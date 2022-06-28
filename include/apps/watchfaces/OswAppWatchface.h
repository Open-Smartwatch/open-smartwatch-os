#pragma once

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"
#ifdef ANIMATION
#include <anim_matrix.h>
#endif

class OswAppWatchface : public OswApp {
  public:
    OswAppWatchface(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppWatchface() {};

#ifdef OSW_FEATURE_STATS_STEPS
    static void drawStepHistory(OswUI* ui, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t max);
#endif
  private:
    OswUI* ui;
    void drawWatch();
#ifdef ANIMATION
    AnimMatrix* matrix;
#endif
};