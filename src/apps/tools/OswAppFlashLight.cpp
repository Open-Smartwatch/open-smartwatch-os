#include "./apps/tools/OswAppFlashLight.h"

#include "config_defaults.h"  // Include the config here again to access the language definitions of it
#include "gfx_util.h"
#include "osw_app.h"
#include "osw_hal.h"
#include "osw_ui.h"

#include <osw_config_keys.h>

void OswAppFlashLight::setup() {}

void OswAppFlashLight::loop() {
    OswHal* hal = OswHal::getInstance();

    static bool on = false;
    static short flashlightBrightness = 255; //seperat variable allows to change the Brightness


    if(hal->btnHasGoneUp(BUTTON_1)) {
        if (on == true) {
            on = false;
        } else {
            on = true;
        }
    }


    if (hal->btnHasGoneDown(BUTTON_3)) {
        flashlightBrightness = flashlightBrightness + 50;
    } else if (hal->btnHasGoneDown(BUTTON_2)) {
        flashlightBrightness = flashlightBrightness - 50;
    }

    draw(flashlightBrightness, on);

    hal->requestFlush();
}

void OswAppFlashLight::draw(short flashlightBrightness, bool on) {
    OswHal* hal = OswHal::getInstance();

    hal->gfx()->fillCircle(120, 120, 120, ui->getSuccessColor());
    if (on == true) {
        hal->setBrightness(flashlightBrightness, 0); //sets the brighntess, but dosent store it
        hal->gfx()->fillCircle(120, 120, 115, ui->getForegroundColor());
        hal->gfx()->setTextSize(3);
        hal->gfx()->setTextCenterAligned();
        hal->gfx()->setTextCursor(120, 125);
        hal->gfx()->setTextColor(ui->getBackgroundColor());
        hal->gfx()->print(int(hal->screenBrightness())); //displays the current brightness

    } else {
        hal->setBrightness(OswConfigAllKeys::settingDisplayBrightness.get()); //sets the brighntess to the initial value
        hal->gfx()->fillCircle(120, 120, 115, ui->getBackgroundColor());
        hal->gfx()->setTextSize(3.5);
        hal->gfx()->setTextCenterAligned();
        hal->gfx()->setTextCursor(120, 125);
        hal->gfx()->setTextColor(ui->getForegroundColor());
        hal->gfx()->print("Flashlight");
    }

}

void OswAppFlashLight::stop() {
    OswHal* hal = OswHal::getInstance();
    hal->setBrightness(OswConfigAllKeys::settingDisplayBrightness.get());  //sets the brighntess to the initial value
}