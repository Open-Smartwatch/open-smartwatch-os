#pragma once

#include <osw_hal.h>
#include <osw_ui.h>

class OswAppWidget {
  public:
#ifdef OSW_FEATURE_STATS_STEPS
    static void drawStepHistory(OswUI* ui, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t max);
#endif
};