#pragma once

#include <osw_hal.h>
#include <OswAppV2.h>

class OswAppExampleV2 : public OswAppV2 {
  public:
    OswAppExampleV2();

    const char* getAppId() override;
    const char* getAppName() override;

    void onStart() override;
    void onLoop() override;
    void onDraw() override;
    void onDrawOverlay() override;
    void onStop() override;
    void onButton(Button id, bool up, ButtonStateNames state) override;

  private:
    // define global scope variables
    bool red = false;
    bool showImage = false;
    unsigned int start = 0;
    unsigned int counter = 0;
    OswImage image;
};
