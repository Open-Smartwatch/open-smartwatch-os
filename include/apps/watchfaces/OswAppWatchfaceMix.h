#ifndef OSW_APP_WATCHFACE_MIX_H
#define OSW_APP_WATCHFACE_MIX_H

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
    OswUI* ui;
};

#endif
