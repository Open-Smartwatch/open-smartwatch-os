#include "./apps/watchfaces/OswAppWatchfaceDual.h"

#include <config.h>
#include <gfx_util.h>
#include <OswAppV1.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <time.h>

#include "./apps/watchfaces/OswAppWatchface.h"
#include "./apps/watchfaces/OswAppWatchfaceDigital.h"

/*!
 @brief draw progress-bar
 @param ui ui interface class
 @param cx Coord X
 @param cy Coord Y
 @param jump interval
 @param length width
 @param value draw the width dynamically
 @param angle angle(rotate)
 @param radius bar-radius
 @param color color
 @param goal specific goal(option)
*/
void OswAppWatchfaceDual::drawProgressBar(OswUI* ui,uint8_t cx, uint8_t cy, uint8_t jump, uint8_t length, uint8_t value,float angle, uint8_t radius, uint16_t color, uint8_t* goal) {
    OswHal* hal = OswHal::getInstance();
    hal->gfx()->drawThickTick(cx, cy, jump, length, angle, radius,  changeColor(color, 0.25f));
    hal->gfx()->drawThickTick(cx, cy, jump, value, angle, radius, goal == nullptr ? color :*goal<value ? ui->getSuccessColor():color, true);
}

void OswAppWatchfaceDual::drawAnimSec() {
    OswHal* hal = OswHal::getInstance();
    uint8_t barWidth = 140;
    uint32_t Hs, Ms, Ss = 0;
    hal->getLocalTime(&Hs,&Ms,&Ss);
    uint32_t onlySecond = Ss;
    uint16_t barValue = ((float)onlySecond / 60) * barWidth;
    barValue = barValue < 2 ? 0 : barValue;
    uint8_t coordX = (DISP_W - barWidth) / 2;
    uint8_t levelY = DISP_H / 2;
    uint8_t radius = 1;
    drawProgressBar(ui, coordX, levelY - 1, 0, barWidth, barValue, 90, radius, ui->getPrimaryColor());
}

const char* OswAppWatchfaceDual::getAppId() {
    return OswAppWatchfaceDual::APP_ID;
}

const char* OswAppWatchfaceDual::getAppName() {
    return LANG_DUAL;
}

void OswAppWatchfaceDual::onStart() {
    OswAppV2::onStart();
    OswAppWatchface::addButtonDefaults(this->knownButtonStates);
}

void OswAppWatchfaceDual::onLoop() {
    OswAppV2::onLoop();

    this->needsRedraw = this->needsRedraw or time(nullptr) != this->lastTime; // redraw every second
}

void OswAppWatchfaceDual::onDraw() {
    OswAppV2::onDraw();

    // Set Dual Size
    hal->gfx()->setTextSize(2);
    uint8_t mid_little = hal->gfx()->getTextOfsetRows(0.5f);
    uint8_t mid = hal->gfx()->getTextOfsetRows(2);

    OswAppWatchfaceDigital::digitalWatch(hal->getTimezoneOffsetPrimary(),1, 120 - mid_little, 120 - mid);
    OswAppWatchfaceDigital::digitalWatch(hal->getTimezoneOffsetSecondary(),1, 120 + mid_little, 120 + mid);

    drawAnimSec();

#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
    OswAppWatchfaceDigital::drawSteps();
#endif

    this->lastTime = time(nullptr);
}

void OswAppWatchfaceDual::onButton(Button id, bool up, OswAppV2::ButtonStateNames state) {
    OswAppV2::onButton(id, up, state);
    if(OswAppWatchface::onButtonDefaults(*this, id, up, state))
        return; // if the button was handled by the defaults, we are done here
}