#include <gfx_util.h>
#include <OswAppV1.h>
#include <osw_hal.h>
#include <osw_ui.h>

#include "apps/examples/OswAppExample.h"

OswAppExample::OswAppExample() {
    // This app also supports double presses (on BUTTON_1), note that this WILL DELAY the reporting of any short press events on that button (as it may needs to wait for the second press)
    this->knownButtonStates[BUTTON_1] = (OswAppV2::ButtonStateNames) (this->knownButtonStates[BUTTON_1] | OswAppV2::ButtonStateNames::DOUBLE_PRESS);
}

const char* OswAppExample::getAppId() {
    return "org.osw.example";
}

const char* OswAppExample::getAppName() {
    return "Example App";
}

void OswAppExample::onStart() {
    OswAppV2::onStart(); // always make sure to call the base class method!

    // This is where you initialise code, gets called before this app is shown
    this->start = time(nullptr); // used as offset for the counter later on
}

void OswAppExample::onLoop() {
    OswAppV2::onLoop(); // always make sure to call the base class method!
    // This section of the code is where you can write the main logic

    // In this example we will just process the button presses to change the color of the text
    // TODO move this into the onButton() method
    bool oldRed = red;
    if (hal->btnHasGoneDown(BUTTON_3))
        red = true;
    if (hal->btnHasGoneDown(BUTTON_2))
        red = false;

    // Then request a redraw if the value has changed or one second passed (for the counter)
    this->needsRedraw = this->needsRedraw or oldRed != red or counter != time(nullptr);
}

void OswAppExample::onDraw() {
    OswAppV2::onDraw(); // always make sure to call the base class method!
    this->counter = time(nullptr); // update the counter

    // As the variable 'red' is changed, this if loop adjusts the colour of the 'hello world' text
    hal->gfx()->setTextCenterAligned();
    if (red)
        hal->gfx()->setTextColor(rgb565(255, 0, 0), OswUI::getInstance()->getBackgroundColor());
    else
        hal->gfx()->setTextColor(rgb565(255, 255, 255), OswUI::getInstance()->getBackgroundColor());

    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextCursor(DISP_W / 2, DISP_H / 2);
    hal->gfx()->print("Hello World");

    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextCursor(DISP_W / 2, DISP_H / 2 + 10); // This places the text exactly into the space of the previous text :)
    hal->gfx()->print(this->counter - this->start);

    if(red) // only reset the text color, if the previous text was red
        hal->gfx()->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));
    hal->gfx()->setTextSize(2);
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
