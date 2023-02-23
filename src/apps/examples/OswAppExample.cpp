#include <gfx_util.h>
#include <OswAppV1.h>
#include <osw_hal.h>

#include "apps/examples/OswAppExample.h"

const char* OswAppExample::getAppId() {
    return "org.osw.example";
}

const char* OswAppExample::getAppName() {
    return "Example App";
}

void OswAppExample::onStart() {
    OswAppV2::onStart(); // always make sure to call the base class method!

    // This is where you initialise code, gets called before this app is shown
}

void OswAppExample::onLoop() {
    OswAppV2::onLoop(); // always make sure to call the base class method!
    // This section of the code is where you can write the main logic

    // In this example we will just process the button presses to change the color of the text
    // TODO move this into the onButton() method
    bool oldRed = red;
    if (hal->btnHasGoneDown(BUTTON_3)) {
        red = true;
    }

    if (hal->btnHasGoneDown(BUTTON_2)) {
        red = false;
    }

    // Then request a redraw if the value has changed
    this->needsRedraw = this->needsRedraw or oldRed != red;
}

void OswAppExample::onDraw() {
    OswAppV2::onDraw(); // always make sure to call the base class method!

    // As the variable 'red' is changed, this if loop adjusts the colour of the 'hello world' text
    hal->gfx()->setTextSize(2);
    if (red)
        hal->gfx()->setTextColor(rgb565(255, 0, 0), rgb565(0, 0, 0));
    else
        hal->gfx()->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));

    hal->gfx()->setTextCursor(50, 120);
    hal->gfx()->print("Hello World");

    if(red) // only reset the text color, if the previous text was red
        hal->gfx()->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));
    hal->gfx()->setTextRightAligned();
    hal->gfx()->setTextCursor(200, 180);
    hal->gfx()->print("Normal");

    hal->gfx()->setTextCursor(200, 60);
    hal->gfx()->print("Red");
}

void OswAppExample::onStop() {
    OswAppV2::onStop(); // always make sure to call the base class method!

    // This is where you de-initialize stuff, gets called when another app is shown
}
