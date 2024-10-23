#include <gfx_util.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>

#include "apps/watchfaces/OswAppWatchface.h"
#include "apps/watchfaces/OswAppWatchfaceBinary.h"
#include OSW_TARGET_PLATFORM_HEADER

#define COLOR_SECxOND rgb565(231, 111, 81)
#define COLOR_MIxNUTE rgb565(244, 162, 97)
#define COLOR_HOxUR rgb565(42, 157, 143)
#define COLOR_BLAxCK rgb565(0, 0, 0)
#define COLOR_WHxITE rgb565(255, 255, 255)

void OswAppWatchfaceBinary::drawWatch() {
    OswHal* hal = OswHal::getInstance();

    OswTime oswTime = { };
    hal->getLocalTime(oswTime);

    uint16_t width = hal->gfx()->getWidth();
    uint16_t height = hal->gfx()->getHeight();

    //hours
    for(uint8_t i = 0; i < 5 ; i++ ) {
        uint32_t b = pow(2, i);
        if((oswTime.hour & b) == 0) {
            hal->gfx()->drawFrame(width - (((width - 32) / 8) * i + 64) - 32, height / 2 - 16, 8, 8, ui->getWarningColor());
        } else {
            hal->gfx()->fillFrame(width - (((width - 32) / 8) * i + 64) - 32, height / 2 - 16, 8, 8, ui->getWarningColor());
        }
    }
    //minutes
    for(uint8_t i = 0; i < 6 ; i++ ) {
        uint32_t b = pow(2, i);
        if((oswTime.minute & b) == 0) {
            hal->gfx()->drawFrame(width - (((width - 32) / 8) * i + 64) - 32, height / 2, 8, 8, ui->getDangerColor());
        } else {
            hal->gfx()->fillFrame(width - (((width - 32) / 8) * i + 64) - 32, height / 2, 8, 8, ui->getDangerColor());
        }
    }
    //seconds
    for(uint8_t i = 0; i < 6 ; i++ ) {
        uint32_t b = pow(2, i);
        if((oswTime.second & b) == 0) {
            hal->gfx()->drawFrame(width - (((width - 32) / 8) * i + 64) - 32, height / 2 + 16, 8, 8, ui->getInfoColor());
        } else {
            hal->gfx()->fillFrame(width - (((width - 32) / 8) * i + 64) - 32, height / 2 + 16, 8, 8, ui->getInfoColor());
        }
    }

    //cosmetic
    hal->gfx()->drawLine(width /2 + 40, height / 8 * 1, width /2 + 40, height / 8 * 7, ui->getForegroundColor());

#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
    //steps
    uint32_t steps = hal->environment()->getStepsToday();
    hal->gfx()->setTextSize(1);
    ui->resetTextColors();
    hal->gfx()->setTextCursor(width /2 + 48, height / 2 + 32 + 16);
    hal->gfx()->print(LANG_WATCHFACE_BINARY_STEPS);

    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextCursor(width /2 + 48, height / 2 + 32);
    hal->gfx()->print("0x");
    hal->gfx()->print(steps, HEX);
#endif
}

const char* OswAppWatchfaceBinary::getAppId() {
    return OswAppWatchfaceBinary::APP_ID;
}

const char* OswAppWatchfaceBinary::getAppName() {
    return LANG_BINARY;
}

void OswAppWatchfaceBinary::onStart() {
    OswAppV2::onStart();
    OswAppWatchface::addButtonDefaults(this->knownButtonStates);
}

void OswAppWatchfaceBinary::onLoop() {
    OswAppV2::onLoop();

    this->needsRedraw = this->needsRedraw or time(nullptr) != this->lastTime; // redraw every second
}

void OswAppWatchfaceBinary::onDraw() {
    OswAppV2::onDraw();

    drawWatch();

    this->lastTime = time(nullptr);
}

void OswAppWatchfaceBinary::onButton(Button id, bool up, OswAppV2::ButtonStateNames state) {
    OswAppV2::onButton(id, up, state);
    if(OswAppWatchface::onButtonDefaults(*this, id, up, state))
        return; // if the button was handled by the defaults, we are done here
}
