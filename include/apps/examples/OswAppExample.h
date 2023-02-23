#pragma once

#include <osw_hal.h>

#include <OswAppV2.h>

class OswAppExample : public OswAppV2 {
  public:
    OswAppExample() {};
    ~OswAppExample() {};

    const char* getAppId() override;
    const char* getAppName() override;

    void onStart() override;
    void onLoop() override;
    void onDraw() override;
    void onStop() override;

  private:
    // define global scope variables
    bool red = false;
};
