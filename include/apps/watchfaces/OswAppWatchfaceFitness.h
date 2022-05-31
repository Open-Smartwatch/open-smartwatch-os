#ifndef OSW_APP_WATCHFACE_FITNESS_H
#define OSW_APP_WATCHFACE_FITNESS_H

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"

class OswAppWatchfaceFitness : public OswApp {
  public:
    OswAppWatchfaceFitness(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppWatchfaceFitness() {};
    static float calculateDistance(uint32_t steps);
    static uint32_t calculateKcalorie(uint32_t steps);

  private:
    void showFitnessTracking();
    OswUI* ui;
};

#endif
