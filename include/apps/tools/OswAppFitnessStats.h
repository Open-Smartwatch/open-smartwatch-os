#ifndef OSW_APP_FITNESS_STATS_H
#define OSW_APP_FITNESS_STATS_H

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"

class OswAppFitnessStats : public OswApp {
  public:
    OswAppFitnessStats(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppFitnessStats() {};
    void showFitnessTracking();

  private:
    OswUI* ui;
};

#endif