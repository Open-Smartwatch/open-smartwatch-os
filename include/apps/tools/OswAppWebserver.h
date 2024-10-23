#pragma once
#ifdef OSW_FEATURE_WIFI

#include <osw_hal.h>
#include <osw_ui.h>
#include <OswAppV1.h>

class OswAppWebserver : public OswApp {
  public:
    OswAppWebserver(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppWebserver() {};

  private:
    OswUI* ui;
    void drawConnectionInfo();
};
#endif
