#include <gfx_util.h>
#include <osw_hal.h>
#include <osw_ui.h>

#include "apps/examples/OswAppExampleV2.h"
#include "assets/img/static/example.png.h"

OswAppExampleV2::OswAppExampleV2(): image(example_png, example_png_length, example_png_width, example_png_height) {

}

const char* OswAppExampleV2::getAppId() {
    return "osw.example.v2";
}

const char* OswAppExampleV2::getAppName() {
    return LANG_EXAMPLE " " LANG_APP " v2";
}

void OswAppExampleV2::onStart() {
    OswAppV2::onStart(); // always make sure to call the base class method!
    // This is where you initialise code, gets called before this app is shown

    // Report that we support short presses on all buttons
    this->knownButtonStates[Button::BUTTON_SELECT] = ButtonStateNames::SHORT_PRESS;
    this->knownButtonStates[Button::BUTTON_UP] = ButtonStateNames::SHORT_PRESS;
    this->knownButtonStates[Button::BUTTON_DOWN] = ButtonStateNames::SHORT_PRESS;

    // Here is a snippet to also "support" double presses (on BUTTON_SELECT) - note that this WILL DELAY the reporting of any short press events on that button (as it may needs to wait for the second press)
    this->knownButtonStates[Button::BUTTON_SELECT] = (OswAppV2::ButtonStateNames) (this->knownButtonStates[Button::BUTTON_SELECT] | OswAppV2::ButtonStateNames::DOUBLE_PRESS); // OR to set the bit
    // ...so we will disable it again ;)
    this->knownButtonStates[Button::BUTTON_SELECT] = (OswAppV2::ButtonStateNames) (this->knownButtonStates[Button::BUTTON_SELECT] ^ OswAppV2::ButtonStateNames::DOUBLE_PRESS); // XOR to toggle the bit

    this->start = time(nullptr); // used as offset for the counter later on
    this->viewFlags = (OswAppV2::ViewFlags) (this->viewFlags | OswAppV2::ViewFlags::KEEP_DISPLAY_ON); // by default we want to keep the display on
}

void OswAppExampleV2::onLoop() {
    OswAppV2::onLoop(); // always make sure to call the base class method!
    // This section of the code is where you can write the main logic

    // We request a redraw if the value has changed or one second passed (for the counter)
    this->needsRedraw = this->needsRedraw or counter != time(nullptr);
}

void OswAppExampleV2::onDraw() {
    OswAppV2::onDraw(); // always make sure to call the base class method!
    this->counter = time(nullptr); // update the counter

    // Maybe draw a background image...
    if(this->showImage)
        this->image.draw(hal->gfx(), DISP_W / 2, DISP_H / 2, 0.8, OswImage::Alignment::CENTER, OswImage::Alignment::CENTER);

    // As the variable 'red' is changed, this if-conditional adjusts the colour of the 'hello world' text
    hal->gfx()->setTextCenterAligned();
    if (red)
        hal->gfx()->setTextColor(rgb565(255, 0, 0)); // this is how you can set a custom color
    else
        hal->gfx()->setTextColor(OswUI::getInstance()->getForegroundColor()); // this is how you can use the "theme" color

    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextCursor(DISP_W / 2, DISP_H / 2);
    hal->gfx()->print(LANG_EXAMPLES_HELLO_WORLD); // This is how you can use the language keys - they are defined in include/locales

    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextCursor(DISP_W / 2, DISP_H / 2 + 10); // This places the text exactly into the space of the previous text :)
    hal->gfx()->print(this->counter - this->start);

    if(red) // only reset the text color, if the previous text was red
        hal->gfx()->setTextColor(OswUI::getInstance()->getForegroundColor());
    OswUI::getInstance()->setTextCursor(BUTTON_UP);
    hal->gfx()->print(LANG_RED);

    OswUI::getInstance()->setTextCursor(BUTTON_DOWN);
    hal->gfx()->print(LANG_NORMAL);

    OswUI::getInstance()->setTextCursor(BUTTON_SELECT);
    hal->gfx()->print(LANG_IMAGE);
}

void OswAppExampleV2::onDrawOverlay() {
    /*
        In this function we do nothing, but here you could draw stuff like an indicator of some kind.
        But... Why are we calling the parents method at last here? Because the parent method may want
        to draw something on top of what we are drawing here. So we need to make sure that we draw our
        stuff first and then call the parent method. This is due to the implicit convention, that this
        method will draw always on TOP of the previous content - hence called "overlay".
    */

    OswAppV2::onDrawOverlay(); // here at last!!! (and always make sure to call the base class method)
}

void OswAppExampleV2::onButton(Button id, bool up, ButtonStateNames state) {
    OswAppV2::onButton(id, up, state); // always make sure to call the base class method!
    if(up and state == ButtonStateNames::SHORT_PRESS) {
        if(id == Button::BUTTON_UP) {
            this->red = true;
            this->needsRedraw = true;
        } else if(id == Button::BUTTON_DOWN) {
            this->red = false;
            this->needsRedraw = true;
        } else if(id == Button::BUTTON_SELECT) {
            this->showImage = !this->showImage;
            this->needsRedraw = true;
        }
    }
}

void OswAppExampleV2::onStop() {
    OswAppV2::onStop(); // always make sure to call the base class method!

    // This is where you de-initialize stuff, gets called when another app is shown
}
