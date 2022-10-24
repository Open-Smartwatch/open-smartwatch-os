#pragma once

#include <osw_hal.h>
#include <osw_ui.h>
#include "osw_app.h"

class OswAppToolsPage2App : public OswApp {
  public:
    OswAppToolsPage2App(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppToolsPage2App() {};

  private:
    OswUI* ui;
};
