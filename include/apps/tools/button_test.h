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
  const char* lastValue = "Press Button";

  const char* goneDown = "Gone Down";
  const char* goneUp = "Gone Up";
  const char* longPress = "Long Press";

  uint8_t lastButton = 255;
};

#endif
