#ifndef OSW_APP_KCAL_STATS_H
#define OSW_APP_KCAL_STATS_H

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"

class OswAppKcalStats : public OswApp {
  public:
    OswAppKcalStats(void) { ui = OswUI::getInstance(); };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppKcalStats(){};

  private:
    uint8_t cursorPos = 6;  // WeekDay position
    void drawCurvedChart();
    void showCurvedChart();
    void readyValue(int type = 0, uint8_t cursor = 6, uint32_t* cursorValue = nullptr);
    uint16_t weekValue[7] = {};
    OswUI* ui;
};

#endif