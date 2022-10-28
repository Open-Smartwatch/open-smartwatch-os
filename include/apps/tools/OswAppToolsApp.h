#pragma once

#include <osw_hal.h>
#include <osw_ui.h>
#include "osw_app.h"

class OswAppToolsApp : public OswApp {
  public:
    OswAppToolsApp(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppToolsApp() {};

  private:
    OswUI* ui;
};
