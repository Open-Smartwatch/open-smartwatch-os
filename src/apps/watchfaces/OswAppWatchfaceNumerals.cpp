
#include "./apps/watchfaces/OswAppWatchfaceNumerals.h"
#include "./fonts/FreeMonoBold12pt7b.h"
#include "./fonts/FreeMonoBold9pt7b.h"
#include "./apps/watchfaces/OswAppWatchfaceDigital.h"
#include "./apps/watchfaces/OswAppWatchfaceMonotimer.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>

void OswAppWatchfaceNumerals::drawWatch() {
    OswHal* hal = OswHal::getInstance();

    hal->gfx()->drawMinuteTicks(120, 120, 116, 112, ui->getForegroundDimmedColor());
    hal->gfx()->drawHourTicks(120, 120, 117, 107, ui->getForegroundColor());

    // hour labels
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextColor(ui->getPrimaryColor());
    hal->gfx()->setFont(&FreeMonoBold9pt7b);
    hal->gfx()->setTextMiddleAligned();

    OswAppWatchfaceMonotimer::drawHour();

    uint32_t dayInt = 0;
    uint32_t monthInt = 0;
    uint32_t yearInt = 0;
    hal->getLocalDate(&dayInt, &monthInt, &yearInt);
    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextColor(ui->getDangerColor());
    hal->gfx()->setTextCursor(120, 85);
    hal->gfx()->print(dayInt);

    const char* weekday = hal->getLocalWeekday();
    hal->gfx()->setTextCursor(120, 70);
    OswAppWatchfaceDigital::displayWeekDay3(weekday);

#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
    uint32_t steps = hal->environment->getStepsToday();
    uint32_t stepsTarget = OswConfigAllKeys::stepsPerDay.get();

    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextCursor(120, 135);
    hal->gfx()->setTextColor(steps>stepsTarget?ui->getSuccessColor():ui->getInfoColor());

    hal->gfx()->setFont(&FreeMonoBold12pt7b);
    hal->gfx()->print(steps);
#endif

    // ticks
    uint32_t second = 0;
    uint32_t minute = 0;
    uint32_t hour = 0;
    hal->getLocalTime(&hour, &minute, &second);
    if(OswConfigAllKeys::settingDisplayDualHourTick.get()) {
        uint32_t dualSecond = 0;
        uint32_t dualMinute = 0;
        uint32_t dualHour = 0;
        hal->getDualTime(&dualHour, &dualMinute, &dualSecond);

        // dual-hours
        hal->gfx()->drawThickTick(120, 120, 0, 16, 360.0 / 12.0 * (1.0 * dualHour + dualMinute / 60.0), 2, ui->getBackgroundDimmedColor());
        hal->gfx()->drawThickTick(120, 120, 16, 60, 360.0 / 12.0 * (1.0 * dualHour + dualMinute / 60.0), 5, ui->getBackgroundDimmedColor());
    }

    // hours
    hal->gfx()->drawThickTick(120, 120, 0, 16, 360.0 / 12.0 * (1.0 * hour + minute / 60.0), 1, ui->getForegroundColor());
    hal->gfx()->drawThickTick(120, 120, 16, 60, 360.0 / 12.0 * (1.0 * hour + minute / 60.0), 4, ui->getForegroundColor());

    // minutes
    hal->gfx()->drawThickTick(120, 120, 0, 16, 360.0 / 60.0 * (1.0 * minute + second / 60.0), 1, ui->getForegroundColor());
    hal->gfx()->drawThickTick(120, 120, 16, 80, 360.0 / 60.0 * (1.0 * minute + second / 60.0), 4, ui->getForegroundColor());

#ifndef GIF_BG
    // seconds
    hal->gfx()->fillCircle(120, 120, 3, ui->getDangerColor());
    hal->gfx()->drawThickTick(120, 120, 0, 16, 180 + ( 360.0 / 60.0 * second ), 1, ui->getDangerColor());
    hal->gfx()->drawThickTick(120, 120, 0, 105, 360.0 / 60.0 * second, 1, ui->getDangerColor());
#endif
}

void OswAppWatchfaceNumerals::setup() {}

void OswAppWatchfaceNumerals::loop() {
    OswHal* hal = OswHal::getInstance();
    if (hal->btnHasGoneDown(BUTTON_3)) {
        hal->increaseBrightness(25);
    }
    if (hal->btnHasGoneDown(BUTTON_2)) {
        hal->decreaseBrightness(25);
    }
    drawWatch();
    hal->requestFlush();
}

void OswAppWatchfaceNumerals::stop() {}
