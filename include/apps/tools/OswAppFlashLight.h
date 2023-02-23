#pragma once

#include <osw_hal.h>
#include <osw_ui.h>
#include <OswAppV1.h>


class OswAppFlashLight : public OswApp {
  public:
    OswAppFlashLight(void) {
        ui = OswUI::getInstance();
    };
    ~OswAppFlashLight() {};
  private:
    OswUI* ui;
    virtual void setup() override;
    virtual void loop() override; //checks for button presses and turns the flashlight on/off
    virtual void stop() override;
    void draw(short flashlightBrightness, bool on);

};