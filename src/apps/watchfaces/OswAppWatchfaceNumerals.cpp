
#include "apps/watchfaces/OswAppWatchfaceNumerals.h"
#include "fonts/FreeMonoBold12pt7b.h"
#include "fonts/FreeMonoBold9pt7b.h"
#include "apps/watchfaces/OswAppWatchfaceDigital.h"
#include "apps/watchfaces/OswAppWatchfaceMonotimer.h"
#include "apps/watchfaces/OswAppWatchface.h"

#include <gfx_util.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>

const char* OswAppWatchfaceNumerals::getAppId() {
    return OswAppWatchfaceNumerals::APP_ID;
}

const char* OswAppWatchfaceNumerals::getAppName() {
    return LANG_NUMERALS;
}

void OswAppWatchfaceNumerals::drawWatch() {
    OswHal* hal = OswHal::getInstance();

    hal->gfx()->drawMinuteTicks(DISP_W / 2, DISP_H / 2, 116, 112, ui->getForegroundDimmedColor());
    hal->gfx()->drawHourTicks(DISP_W / 2, DISP_H / 2, 117, 107, ui->getForegroundColor());

    // hour labels
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextColor(ui->getPrimaryColor());
    hal->gfx()->setFont(&FreeMonoBold9pt7b);
    hal->gfx()->setTextMiddleAligned();

    OswAppWatchfaceMonotimer::drawHour();

    OswDate oswDate = { };
    hal->getLocalDate(oswDate);
    uint32_t dayInt = oswDate.day;
    uint32_t monthInt = oswDate.month;
    uint32_t yearInt = oswDate.year;
    
    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextColor(ui->getDangerColor());
    hal->gfx()->setTextCursor(120, 85);
    hal->gfx()->print(dayInt);

    const char* weekday = oswDate.weekDayName;
    hal->gfx()->setTextCursor(120, 70);
    OswAppWatchfaceDigital::displayWeekDay3(weekday);

#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
    uint32_t steps = hal->environment()->getStepsToday();
    uint32_t stepsTarget = OswConfigAllKeys::stepsPerDay.get();

    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextCursor(120, 135);
    hal->gfx()->setTextColor(steps>stepsTarget?ui->getSuccessColor():ui->getInfoColor());

    hal->gfx()->setFont(&FreeMonoBold12pt7b);
    hal->gfx()->print(steps);
#endif

    // ticks
    OswTime oswTime = { };
    hal->getLocalTime(oswTime);
    uint32_t second = oswTime.second;
    uint32_t minute = oswTime.minute;
    uint32_t hour = oswTime.hour;
    if(OswConfigAllKeys::settingDisplayDualHourTick.get()) {
        OswTime oswTime = { };
        hal->getDualTime(oswTime);
        uint32_t dualSecond = oswTime.second;
        uint32_t dualMinute = oswTime.minute;
        uint32_t dualHour = oswTime.hour;

        // dual-hours
        hal->gfx()->drawThickTick(DISP_W / 2, DISP_H / 2, 0, 16, 360.0f / 12.0f * (dualHour + dualMinute / 60.0f), 2, ui->getBackgroundDimmedColor());
        hal->gfx()->drawThickTick(DISP_W / 2, DISP_H / 2, 16, 60, 360.0f / 12.0f * (dualHour + dualMinute / 60.0f), 5, ui->getBackgroundDimmedColor());
    }

    // hours
    hal->gfx()->drawThickTick(DISP_W / 2, DISP_H / 2, 0, 16, 360.0f / 12.0f * (hour + minute / 60.0f), 1, ui->getForegroundColor());
    hal->gfx()->drawThickTick(DISP_W / 2, DISP_H / 2, 16, 60, 360.0f / 12.0f * (hour + minute / 60.0f), 4, ui->getForegroundColor());

    // minutes
    hal->gfx()->drawThickTick(DISP_W / 2, DISP_H / 2, 0, 16, 360.0f / 60.0f * (minute + second / 60.0f), 1, ui->getForegroundColor());
    hal->gfx()->drawThickTick(DISP_W / 2, DISP_H / 2, 16, 80, 360.0f / 60.0f * (minute + second / 60.0f), 4, ui->getForegroundColor());

#ifndef GIF_BG
    // seconds
    hal->gfx()->fillCircle(DISP_W / 2, DISP_H / 2, 3, ui->getDangerColor());
    hal->gfx()->drawThickTick(DISP_W / 2, DISP_H / 2, 0, 16, 180 + ( 360.0f / 60.0f * second ), 1, ui->getDangerColor());
    hal->gfx()->drawThickTick(DISP_W / 2, DISP_H / 2, 0, 105, 360.0f / 60.0f * second, 1, ui->getDangerColor());
#endif
}

void OswAppWatchfaceNumerals::onStart() {
    OswAppV2::onStart();
    OswAppWatchface::addButtonDefaults(this->knownButtonStates);
}

void OswAppWatchfaceNumerals::onLoop() {
    OswAppV2::onLoop();

    this->needsRedraw = this->needsRedraw or time(nullptr) != this->lastTime; // redraw every second
}

void OswAppWatchfaceNumerals::onDraw() {
    OswAppV2::onDraw();

    drawWatch();

    this->lastTime = time(nullptr);
}

void OswAppWatchfaceNumerals::onButton(Button id, bool up, OswAppV2::ButtonStateNames state) {
    OswAppV2::onButton(id, up, state);
    if(OswAppWatchface::onButtonDefaults(*this, id, up, state))
        return; // if the button was handled by the defaults, we are done here
}
