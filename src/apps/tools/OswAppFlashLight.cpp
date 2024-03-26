#include "apps/tools/OswAppFlashLight.h"
#include "assets/img/icons/flashlight.png.h"

OswAppFlashLight::OswAppFlashLight(): OswAppV2() {

}

OswAppFlashLight::~OswAppFlashLight() {

}

const char* OswAppFlashLight::getAppId() {
    return "osw.flashlight";
}

const char* OswAppFlashLight::getAppName() {
    return "Flashlight";
}

const OswIcon& OswAppFlashLight::getAppIcon() {
    return flashlight_png;
}

void OswAppFlashLight::onStart() {
    OswAppV2::onStart();
    this->knownButtonStates[Button::BUTTON_SELECT] = ButtonStateNames::SHORT_PRESS;
    this->knownButtonStates[Button::BUTTON_UP] = ButtonStateNames::SHORT_PRESS;
    this->knownButtonStates[Button::BUTTON_DOWN] = ButtonStateNames::SHORT_PRESS;
}

void OswAppFlashLight::onDraw() {
    OswAppV2::onDraw();

    this->hal->gfx()->fillCircle(120, 120, 120, ui->getSuccessColor());
    if (this->on) {
        this->hal->gfx()->fillCircle(120, 120, 115, ui->getForegroundColor());
        this->hal->gfx()->setTextSize(3);
        this->hal->gfx()->setTextCenterAligned();
        this->hal->gfx()->setTextCursor(120, 125);
        this->hal->gfx()->setTextColor(ui->getBackgroundColor());
        this->hal->gfx()->print(int(hal->screenBrightness())); //displays the current brightness
    } else {
        this->hal->gfx()->fillCircle(120, 120, 115, ui->getBackgroundColor());
        this->hal->gfx()->setTextSize(3);
        this->hal->gfx()->setTextCenterAligned();
        this->hal->gfx()->setTextCursor(120, 125);
        this->hal->gfx()->setTextColor(ui->getForegroundColor());
        this->hal->gfx()->print("Flashlight");
    }
}

void OswAppFlashLight::onButton(Button id, bool up, OswAppV2::ButtonStateNames state) {
    OswAppV2::onButton(id, up, state); // always make sure to call the base class method!
    if(!up) return;
    this->needsRedraw = true; // we need to redraw the screen, regardless of what happens next
    if(id == Button::BUTTON_SELECT) {
        this->on = !this->on;
        // whenever the flashlight is active, we should not let the screen go to sleep
        if(this->on)
            this->viewFlags = (OswAppV2::ViewFlags) (this->viewFlags | OswAppV2::ViewFlags::KEEP_DISPLAY_ON); // on
        else
            this->viewFlags = (OswAppV2::ViewFlags) (this->viewFlags ^ OswAppV2::ViewFlags::KEEP_DISPLAY_ON); // toggle (on->off)
    }
    // if flashlight is active, allow brightness adjustment
    if(this->on) {
        if(id == Button::BUTTON_UP)
            this->flashlightBrightness = this->flashlightBrightness + 50;
        else if(id == Button::BUTTON_DOWN)
            this->flashlightBrightness = this->flashlightBrightness - 50;
        this->hal->setBrightness(flashlightBrightness, false); // sets the brightness to the current value
    } else {
        this->hal->setBrightness(OswConfigAllKeys::settingDisplayBrightness.get()); //sets the brightness to the initial value
    }
}

void OswAppFlashLight::onStop() {
    OswAppV2::onStop();
    this->hal->setBrightness(OswConfigAllKeys::settingDisplayBrightness.get(), false); // reset the brightness to the initial value
}