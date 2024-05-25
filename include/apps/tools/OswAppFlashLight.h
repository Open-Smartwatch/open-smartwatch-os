#pragma once

#include <osw_hal.h>
#include <osw_ui.h>
#include <OswAppV2.h>

class OswAppFlashLight : public OswAppV2 {
  public:
    OswAppFlashLight();
    virtual ~OswAppFlashLight();

    const char* getAppId() override;
    const char* getAppName() override;
    const OswIcon& getAppIcon() override;

    void onStart() override;
    void onDraw() override;
    void onStop() override;

    virtual void onButton(Button id, bool up, OswAppV2::ButtonStateNames state) override;
  private:
    bool on = false;
    short flashlightBrightness = 255; // separate variable allows to change the Brightness
};
