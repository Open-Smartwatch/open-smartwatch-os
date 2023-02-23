#pragma once

#include <osw_hal.h>
#include <osw_ui.h>

#include <OswAppV1.h>

class OswAppWatchfaceBinary : public OswApp {
  public:
    OswAppWatchfaceBinary(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppWatchfaceBinary() {};

  private:
    uint16_t primaryColor;
    void drawWatch();
    OswUI* ui;
};