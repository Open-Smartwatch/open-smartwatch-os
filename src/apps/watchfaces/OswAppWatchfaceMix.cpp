#include "./apps/watchfaces/OswAppWatchfaceDigital.h"
#include "./apps/watchfaces/OswAppWatchfaceDual.h"
#include "./apps/watchfaces/OswAppWatchfaceMix.h"
#include "./apps/watchfaces/OswAppWatchface.h"

#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include <time.h>

#define OFF_SET_ANALOG_WATCH_X_COORD        62      // need to set coord, Use this part to move to the x-axis.
#define OFF_SET_DATE_DIGITAL_WATCH_X_COORD   7      // need to set coord, Use this part to move to the x-axis.

void OswAppWatchfaceMix::analogWatchDisplay() {
    OswHal* hal = OswHal::getInstance();
    uint32_t second = 0;
    uint32_t minute = 0;  // Unused, but required by function signature
    uint32_t hour = 0;    // Unused, but required by function signature

    hal->getLocalTime(&hour, &minute, &second);
    hal->gfx()->drawCircle((int)(DISP_W*0.5)-OFF_SET_ANALOG_WATCH_X_COORD, 100, 50, ui->getForegroundColor());
    hal->gfx()->drawHourTicks((int)(DISP_W*0.5)-OFF_SET_ANALOG_WATCH_X_COORD, 100, 45, 40, ui->getForegroundDimmedColor());

    // hour
    hal->gfx()->drawLine((int)(DISP_W*0.5)-OFF_SET_ANALOG_WATCH_X_COORD, 100, rpx((int)(DISP_W*0.5)-OFF_SET_ANALOG_WATCH_X_COORD, (int)(33 * 0.5), hour * 30  + (int)( minute* 0.1 ) * 6), rpy(100, (int)(33 * 0.5), hour * 30 + (int)( minute* 0.1 ) * 6 ), ui->getForegroundColor());
    // minute
    hal->gfx()->drawLine((int)(DISP_W*0.5)-OFF_SET_ANALOG_WATCH_X_COORD, 100, rpx((int)(DISP_W*0.5)-OFF_SET_ANALOG_WATCH_X_COORD, (int)(66 * 0.5),  minute * 6), rpy(100, (int)(66 * 0.5),  minute * 6), ui->getSuccessColor());
    // second
    hal->gfx()->drawLine((int)(DISP_W*0.5)-OFF_SET_ANALOG_WATCH_X_COORD, 100, rpx((int)(DISP_W*0.5)-OFF_SET_ANALOG_WATCH_X_COORD, (int)(15 * 0.5), s2d(second) + 180), rpy(100, (int)(15 * 0.5), s2d(second) + 180), ui->getDangerColor());  // short backwards
    hal->gfx()->drawLine((int)(DISP_W*0.5)-OFF_SET_ANALOG_WATCH_X_COORD, 100, rpx((int)(DISP_W*0.5)-OFF_SET_ANALOG_WATCH_X_COORD, (int)(90 * 0.5), s2d(second)), rpy(100, (int)(90 * 0.5), s2d(second)), ui->getDangerColor());  // long front
}

void OswAppWatchfaceMix::dateDisplay() {
    uint32_t dayInt = 0;
    uint32_t monthInt = 0;
    uint32_t yearInt = 0;
    OswHal* hal = OswHal::getInstance();
    const char* weekday = hal->getLocalWeekday();

    hal->getLocalDate(&dayInt, &monthInt, &yearInt);

    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextCursor(DISP_W / 2 - OFF_SET_DATE_DIGITAL_WATCH_X_COORD, 75);

    OswAppWatchfaceDigital::displayWeekDay3(weekday);

    hal->gfx()->print(", ");

    // Date
    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextCursor(DISP_W / 2 - OFF_SET_DATE_DIGITAL_WATCH_X_COORD, 90);

    OswAppWatchfaceDigital::dateOutput(yearInt, monthInt, dayInt);
}

void OswAppWatchfaceMix::digitalWatchDisplay() {
    uint32_t second = 0;
    uint32_t minute = 0;
    uint32_t hour = 0;
    bool afterNoon = false;
    char am[] = "AM";
    char pm[] = "PM";
    OswHal* hal = OswHal::getInstance();

    hal->gfx()->setTextSize(3);
    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextCursor(DISP_W / 2 - OFF_SET_DATE_DIGITAL_WATCH_X_COORD, DISP_H / 2);

    hal->getLocalTime(&hour, &minute, &second, &afterNoon);
    OswAppWatchfaceDigital::timeOutput(hour, minute, second,false);
    if (!OswConfigAllKeys::timeFormat.get()) {
        hal->gfx()->setTextSize(1);
        hal->gfx()->setTextMiddleAligned();
        hal->gfx()->setTextLeftAligned();
        hal->gfx()->setTextBottomAligned();
        hal->gfx()->setTextSize(3);
        hal->gfx()->setTextCursor(DISP_W / 2 - OFF_SET_DATE_DIGITAL_WATCH_X_COORD + hal->gfx()->getTextOfsetColumns(5.25), 130);
        hal->gfx()->setTextSize(1);
        hal->gfx()->print(" ");
        if (afterNoon) {
            hal->gfx()->print(pm);
        } else {
            hal->gfx()->print(am);
        }
    }
}

void OswAppWatchfaceMix::setup() {
    uint32_t nowDay, dayOfMonth = 0;
    OswHal::getInstance()->getLocalDate(&dayOfMonth, &nowDay);
    pos = nowDay;
}
void OswAppWatchfaceMix::loop() {
    OswHal* hal = OswHal::getInstance();
    if (hal->btnIsDown(BUTTON_3) && hal->btnHasGoneDown(BUTTON_2)) {
        buttonMode = (buttonMode + 1) % 2;
    }
    ui->watchfaceBrightnessControlMode = buttonMode;

    switch (buttonMode) {
    case 0:
        OswAppWatchface::settingBrightness();
        break;
    default:
        //  Your feature
        if (hal->btnHasGoneDown(BUTTON_3)) {
            this->pos = this->pos + 1 > 6 ? 6 : this->pos + 1;
        }
        if (hal->btnHasGoneDown(BUTTON_2)) {
            this->pos = this->pos - 1 < 0 ? 0 : this->pos - 1;
        }
        break;
    }

    analogWatchDisplay();
    dateDisplay();
    digitalWatchDisplay();
#ifdef OSW_FEATURE_STATS_STEPS
    OswAppWatchface::drawStepsFrame(ui,pos);
#endif

    hal->requestFlush();
}

void OswAppWatchfaceMix::stop() {

}
