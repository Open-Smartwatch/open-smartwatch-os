
#include "apps/watchfaces/OswAppWatchfaceMonotimer.h"
#include "apps/watchfaces/OswAppWatchface.h"
// #define GIF_BG

#include <gfx_util.h>
#include <OswAppV1.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>

#ifdef GIF_BG
#include "./apps/_experiments/gif_player.h"
#endif

/**
 * @brief Draw N shifted ticks around the clock.
 *
 * @param cx center x axis
 * @param cy center y axis
 * @param r1 radius from the begin of the tick.
 * @param r2 radius from the end of the tick.
 * @param nTicks number of ticks to draw
 * @param shift amount of degrees the ticks are shifted clockwise from the top (0°)
 * @param color color code
 */
void OswAppWatchfaceMonotimer::drawNShiftedTicks(Graphics2D* gfx, uint8_t cx, uint8_t cy, uint8_t r1, uint8_t r2, uint8_t nTicks, float shift, uint16_t color) {
    float deltaAngle = 360.0f / nTicks;
    for (uint16_t i=0; i<nTicks; ++i) {
        gfx->drawTick(cx, cy, r1, r2, (i * deltaAngle) + shift, color);
    }
}

/**
 * @brief Draw N shifted ticks around the clock; skip every m-th.
 *
 * @param cx center x axis
 * @param cy center y axis
 * @param r1 radius from the begin of the tick.
 * @param r2 radius from the end of the tick.
 * @param nTicks number of ticks to draw
 * @param shift amount of degrees the ticks are shifted clockwise from the top (0°)
 * @param m do not draw every m-th tick
 * @param color color code
 */
void OswAppWatchfaceMonotimer::drawNShiftedMaskedTicks(Graphics2D* gfx, uint8_t cx, uint8_t cy, uint8_t r1, uint8_t r2, uint8_t nTicks, float shift, uint16_t m, uint16_t color) {
    float deltaAngle = 360.0f / nTicks;
    for (uint16_t i=0; i<nTicks; ++i) {
        if (i % m) {
            gfx->drawTick(cx, cy, r1, r2, (i * deltaAngle) + shift, color);
        }
    }
}
void OswAppWatchfaceMonotimer::drawHour() {
    OswHal* hal = OswHal::getInstance();
    static uint8_t positions[] = {
        155, 45,  // 01
        180, 75,  // 02
        200, 120, // 03
        183, 165, // 04
        155, 197, // 05
        110, 210, // 06
        65, 197,  // 07
        37, 165,  // 08
        23, 120,  // 09
        37, 75,   // 10
        65, 45,   // 11
        110, 30   // 12
    };

    for (uint8_t i = 0; i < 12; ++i) {
        hal->gfx()->setTextCursor(positions[i << 1], positions[(i << 1) | 1]);
        hal->gfx()->printDecimal(i + 1, 2);
    }
}
void OswAppWatchfaceMonotimer::drawWatch() {
    OswHal* hal = OswHal::getInstance();

    // hours
    hal->gfx()->drawNTicks(120, 120, 117, 100, 12, ui->getForegroundColor());
    // 30 minutes
    drawNShiftedTicks(hal->gfx(), 120, 120, 117, 105, 12, 360.0f/24.0f, ui->getForegroundColor());
    // 15 minutes
    drawNShiftedTicks(hal->gfx(), 120, 120, 110, 105, 24, 360.0f/48.0f, ui->getForegroundColor());
    // 5 minutes
    drawNShiftedMaskedTicks(hal->gfx(), 120, 120, 109, 108, 144, 0.0f, 3, ui->getForegroundColor());

    // hour labels
    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextMiddleAligned();

    OswAppWatchfaceMonotimer::drawHour();

#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
    uint32_t steps = hal->environment()->getStepsToday();
    uint32_t stepsTarget = OswConfigAllKeys::stepsPerDay.get();

    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextSize(1);
    hal->gfx()->setTextCursor(120, 135);
    hal->gfx()->setTextColor(steps>stepsTarget?ui->getSuccessColor():ui->getInfoColor());

    hal->gfx()->print(steps);
#endif

    // ticks
    uint32_t second = 0;
    uint32_t minute = 0;
    uint32_t hour = 0;
    hal->getLocalTime(&hour, &minute, &second);

    if (OswConfigAllKeys::settingDisplayDualHourTick.get()) {
        uint32_t dualSecond = 0;
        uint32_t dualMinute = 0;
        uint32_t dualHour = 0;
        hal->getDualTime(&dualHour, &dualMinute, &dualSecond);

        hal->gfx()->drawThickTick(120, 120, 0, 105, (360.0f * (60 * dualHour + dualMinute)) / 720.0f, 1, ui->getBackgroundDimmedColor());
    }

    hal->gfx()->drawThickTick(120, 120, 0, 105, (360.0f * (60 * hour + minute)) / 720.0f, 1, ui->getForegroundColor());

    hal->gfx()->fillEllipse(120, 120, 4, 4, ui->getForegroundColor());
}

#ifdef GIF_BG
static OswAppGifPlayer* bgGif = new OswAppGifPlayer();
#endif

const char* OswAppWatchfaceMonotimer::getAppId() {
    return OswAppWatchfaceMonotimer::APP_ID;
}

const char* OswAppWatchfaceMonotimer::getAppName() {
    return LANG_MONO;
}

void OswAppWatchfaceMonotimer::onStart() {
    OswAppV2::onStart();
    OswAppWatchface::addButtonDefaults(this->knownButtonStates);
#ifdef GIF_BG
    bgGif->setup();
#endif
}

void OswAppWatchfaceMonotimer::onLoop() {
    OswAppV2::onLoop();

    this->needsRedraw = this->needsRedraw or time(nullptr) != this->lastTime; // redraw every second
}

void OswAppWatchfaceMonotimer::onDraw() {
    OswAppV2::onDraw();

#ifdef GIF_BG
    bgGif->loop();
#endif

    drawWatch();

    this->lastTime = time(nullptr);
}

void OswAppWatchfaceMonotimer::onStop() {
    OswAppV2::onStop();
#ifdef GIF_BG
    bgGif->stop();
#endif
}

void OswAppWatchfaceMonotimer::onButton(Button id, bool up, OswAppV2::ButtonStateNames state) {
    OswAppV2::onButton(id, up, state);
    if(OswAppWatchface::onButtonDefaults(*this, id, up, state))
        return; // if the button was handled by the defaults, we are done here
}