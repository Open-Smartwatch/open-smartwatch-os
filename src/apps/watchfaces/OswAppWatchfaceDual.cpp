#include "./apps/watchfaces/OswAppWatchfaceDual.h"

#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
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
    hal->gfx()->drawThickTick(cx, cy, jump, length, angle, radius,  changeColor(color, 0.25));
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
    uint8_t radius = 1.5;
    drawProgressBar(ui, coordX, levelY - 1, 0, barWidth, barValue, 90, radius, ui->getPrimaryColor());
}

void OswAppWatchfaceDual::setup() {}

void OswAppWatchfaceDual::loop() {
    OswHal* hal = OswHal::getInstance();
    OswAppWatchface::handleButtonDefaults();

    // Set Dual Size
    hal->gfx()->setTextSize(2);
    uint8_t mid_little = hal->gfx()->getTextOfsetRows(0.5);
    uint8_t mid = hal->gfx()->getTextOfsetRows(2);

    OswAppWatchfaceDigital::digitalWatch(hal->getTimezoneOffsetPrimary(),1, 120 - mid_little, 120 - mid);
    OswAppWatchfaceDigital::digitalWatch(hal->getTimezoneOffsetSecondary(),1, 120 + mid_little, 120 + mid);

    drawAnimSec();

#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
    OswAppWatchfaceDigital::drawSteps();
#endif
}

void OswAppWatchfaceDual::stop() { }