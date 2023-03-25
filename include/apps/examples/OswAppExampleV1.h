#pragma once

#include <osw_hal.h>
#include <OswAppV1.h>

class OswAppExampleV1 : public OswAppV1 {
  public:
    OswAppExampleV1();

    void setup() override;
    void loop() override;
    void stop() override;

  private:
    // define global scope variables
    bool red = false;
    unsigned int start = 0;
    unsigned int counter = 0;
};
