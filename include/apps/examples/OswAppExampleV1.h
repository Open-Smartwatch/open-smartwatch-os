#pragma once

#include <OswAppV1.h>
#include <OswImage.h>

class OswAppExampleV1 : public OswAppV1 {
  public:
    OswAppExampleV1();

    void setup() override;
    void loop() override;
    void stop() override;

  private:
    // define global scope variables
    bool red = false;
    bool showImage = false;
    unsigned int start = 0;
    unsigned int counter = 0;
    OswImage image;
};
