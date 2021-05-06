#ifndef OSW_BUTTON_TEST_H
#define OSW_BUTTON_TEST_H

#include <osw_hal.h>

#include "osw_app.h"

class OswButtonTest : public OswApp {
 public:
  OswButtonTest(void){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswButtonTest(){};

 private:
    char* lastValue = "Press Button";

    char* goneDown = "Gone Down";
    char* goneUp = "Gone Up";
    char* longPress = "Long Press";

    uint8_t lastButton = 255;
};

#endif