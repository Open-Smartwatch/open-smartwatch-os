
#include "./apps/watchfaces/OswAppWatchfaceNumerals.h"
//#define GIF_BG 1
//#define ANIMATION 1
#include "./fonts/FreeMonoBold12pt7b.h"
#include "./fonts/FreeMonoBold9pt7b.h"

#ifdef ANIMATION
#include <animations/anim_matrix.h>
#endif

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>

#ifdef GIF_BG
#include "./apps/_experiments/gif_player.h"
#endif

// shorten Weekday to 3 charater
void OswAppWatchfaceNumerals::displayWeekDay3(const char* weekday) {
    OswHal* hal = OswHal::getInstance();

    char weekday3[4];
    weekday3[0] = weekday[0];
    weekday3[1] = weekday[1];
    weekday3[2] = weekday[2];
    weekday3[3] = '\0';
    hal->gfx()->setTextCursor(120, 70);
    hal->gfx()->print(weekday3);
}


void OswAppWatchfaceNumerals::drawWatch() {
    OswHal* hal = OswHal::getInstance();

    hal->gfx()->drawMinuteTicks(120, 120, 116, 112, ui->getForegroundDimmedColor());
    hal->gfx()->drawHourTicks(120, 120, 117, 107, ui->getForegroundColor());

    // hour labels
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextColor(ui->getPrimaryColor());
    hal->gfx()->setFont(&FreeMonoBold9pt7b);
    hal->gfx()->setTextMiddleAligned();

    static uint8_t positions[]= {
        155, 45,    // 01
        183, 80,    // 02
        200, 120,   // 03
        183, 165,   // 04
        155, 197,   // 05
        110, 210,   // 06
        65, 197,    // 07
        37, 165,    // 08
        21, 120,    // 09
        37, 80,     // 10
        65, 45,     // 11
        110, 30     // 12
    };

    for (uint8_t i=0; i<12; ++i) {
        hal->gfx()->setTextCursor(positions[i<<1], positions[(i<<1)|1]);
        hal->gfx()->printDecimal(i+1, 2);
    }

    uint32_t dayInt = 0;
    uint32_t monthInt = 0;
    uint32_t yearInt = 0;
    short timeZone = OswConfigAllKeys::timeZone.get();
    const char* weekday = hal->getWeekday(timeZone);
    hal->getDate(timeZone,&dayInt, &monthInt, &yearInt);
    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextColor(ui->getDangerColor());
    hal->gfx()->setTextCursor(120, 85);
    hal->gfx()->print(dayInt);
    OswAppWatchfaceNumerals::displayWeekDay3(weekday);

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

#ifdef GIF_BG
OswAppGifPlayer* bgGif = new OswAppGifPlayer();
#endif

void OswAppWatchfaceNumerals::setup() {
#ifdef GIF_BG
    bgGif->setup();
#endif
#ifdef ANIMATION
    // create new animation object adapted for OSW screen
    this->matrix = new AnimMatrix(OswHal::getInstance()->gfx(), "GATC", 4, 16, 2);
#endif
}

void OswAppWatchfaceNumerals::loop() {
    OswHal* hal = OswHal::getInstance();
    if (hal->btnHasGoneDown(BUTTON_3)) {
        hal->increaseBrightness(25);
    }
    if (hal->btnHasGoneDown(BUTTON_2)) {
        hal->decreaseBrightness(25);
    }

#ifdef GIF_BG
    bgGif->loop();
#endif

#ifdef ANIMATION
    matrix->loop(hal->gfx());
#endif
    drawWatch();
    hal->requestFlush();
}

void OswAppWatchfaceNumerals::stop() {
#ifdef GIF_BG
    bgGif->stop();
#endif
}
