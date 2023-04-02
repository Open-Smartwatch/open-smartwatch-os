
#include "./apps/tools/button_test.h"

#include <gfx_util.h>
#include <OswAppV1.h>
#include <osw_hal.h>

void OswButtonTest::setup() {}

void OswButtonTest::loop() {
    OswHal* hal = OswHal::getInstance();
    for (uint8_t i = 0; i < BTN_NUMBER; i++) {
        if (hal->btnHasGoneDown((Button) i)) {
            lastValue = goneDown;
            lastButton = i;
        }
        if (hal->btnHasGoneUp((Button) i)) {
            lastValue = goneUp;
            lastButton = i;
        }
        if (hal->btnIsLongPress((Button) i)) {
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
}

void OswButtonTest::stop() {}
