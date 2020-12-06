
#include "osw_app_hello_world.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

// OswAppHelloWorld::OswAppHelloWorld(void) : OswApp() {}

void OswAppHelloWorld::loop(OswHal* hal) {
  static long loopCount = 0;
  loopCount++;
  hal->getCanvas()->fillScreen(0);
  hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
  hal->getCanvas()->setCursor(24, 119);
  hal->getCanvas()->print("Hello World ");
  hal->getCanvas()->print(loopCount);
}
