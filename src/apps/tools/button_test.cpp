
#include "./apps/tools/button_test.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

void OswButtonTest::setup() {}

void OswButtonTest::loop() {
  OswHal* hal = OswHal::getInstance();
  for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
    if (hal->btnHasGoneDown(hal->buttons[i])) {
      lastValue = goneDown;
      lastButton = i;
    }
    if (hal->btnHasGoneUp(hal->buttons[i])) {
      lastValue = goneUp;
      lastButton = i;
    }
    if (hal->btnIsLongPress(hal->buttons[i])) {
      lastValue = longPress;
      lastButton = i;
    }
  }

  hal->gfx()->fill(0);
  hal->gfx()->setTextSize(4);
  hal->gfx()->setTextCursor(40, 120);
  hal->gfx()->print(lastValue);
  if (lastButton < 255) {
    hal->gfx()->setTextCursor(40, 60);
    hal->gfx()->print(lastButton);
  }
  hal->requestFlush();
}

void OswButtonTest::stop() {}
