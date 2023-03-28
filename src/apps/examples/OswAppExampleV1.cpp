#include <gfx_util.h>
#include <osw_hal.h>
#include <osw_ui.h>

#include "apps/examples/OswAppExampleV1.h"
#include "assets/img/static/example.png.h"

OswAppExampleV1::OswAppExampleV1(): image(example_png, example_png_length, example_png_width, example_png_height) {

}

void OswAppExampleV1::setup() {
    // This is where you initialise code, gets called before this app is shown
    this->start = time(nullptr); // used as offset for the counter later on
}

void OswAppExampleV1::loop() {
    // This section of the code is where you can write the main logic
    OswHal* hal = OswHal::getInstance(); // Do not cache this reference in e.g. your constructor, as it may change!

    // In this example we will just process the button presses to change the color of the text
    if (hal->btnHasGoneDown(BUTTON_UP))
        red = true;
    if (hal->btnHasGoneDown(BUTTON_DOWN))
        red = false;
    if (hal->btnHasGoneDown(BUTTON_SELECT))
        this->showImage = !this->showImage;

    this->counter = time(nullptr); // update the counter

    // Maybe draw a background image...
    if(this->showImage)
        this->image.draw(hal->gfx(), DISP_W / 2, DISP_H / 2, 0.8, OswImage::Alignment::CENTER, OswImage::Alignment::CENTER);

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
        hal->gfx()->setTextColor(rgb565(255, 255, 255), OswUI::getInstance()->getBackgroundColor());
    OswUI::getInstance()->setTextCursor(BUTTON_UP);
    hal->gfx()->print("Red");

    OswUI::getInstance()->setTextCursor(BUTTON_DOWN);
    hal->gfx()->print("Normal");

    OswUI::getInstance()->setTextCursor(BUTTON_SELECT);
    hal->gfx()->print("Image");
}

void OswAppExampleV1::stop() {
    // This is where you de-initialize stuff, gets called when another app is shown
}
