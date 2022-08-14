
#include "./apps/watchfaces/OswAppWatchface.h"
// #define GIF_BG

#ifdef ANIMATION
#include <anim_matrix.h>
#endif

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>

#ifdef GIF_BG
#include "./apps/_experiments/gif_player.h"
#endif

#ifdef OSW_FEATURE_STATS_STEPS
void OswAppWatchface::drawStepHistory(OswUI* ui, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t max,uint8_t posCursor) {
    OswHal* hal = OswHal::getInstance();
    OswUI::getInstance()->resetTextColors();
    uint32_t weekDay = 0;
    uint32_t dayOfMonth = 0;
    hal->getLocalDate(&dayOfMonth, &weekDay);
    for (uint8_t index = 0;index < 7; index++) {
        uint32_t s = hal->environment->getStepsOnDay(index);
        uint16_t boxHeight = ((float)(s > max ? max : s) / max) * h;
        boxHeight = boxHeight < 2 ? 0 : boxHeight;

        // step bars
        uint16_t c = OswConfigAllKeys::stepsPerDay.get() <= s ? ui->getSuccessColor() : ui->getPrimaryColor();
        hal->gfx()->fillFrame(x + index * w, y + (h - boxHeight), w, boxHeight, c);
        // bar frames
        uint16_t f = posCursor == index ? ui->getForegroundColor() : ui->getForegroundDimmedColor();
        hal->gfx()->drawRFrame(x + index * w, y, w, h, 2, f);

        // labels
        hal->gfx()->setTextCenterAligned();  // horiz.
        hal->gfx()->setTextBottomAligned();
        hal->gfx()->setTextSize(1);
        hal->gfx()->setTextCursor(DISP_W / 2, y - 1);

        if (OswConfigAllKeys::settingDisplayStepsGoal.get()) {
            hal->gfx()->print(hal->environment->getStepsOnDay(posCursor) + String("/") + max);
        } else {
            hal->gfx()->print(hal->environment->getStepsOnDay(posCursor));
        }
        hal->gfx()->setTextCursor(DISP_W / 2, y + 1 + 8 + w * 4);
        hal->gfx()->setTextColor(ui->getForegroundColor());  // Let's make the background transparent.
        // See : https://github.com/Open-Smartwatch/open-smartwatch-os/issues/194
        // font : WHITE / bg : None
        hal->gfx()->print(hal->environment->getStepsTotal());
        hal->gfx()->setTextColor(ui->getForegroundColor(), ui->getBackgroundColor());  // restore. font : WHITE / bg : BLACK
    }
}
void OswAppWatchface::drawStepsFrame(OswUI *ui, uint8_t pos) {
    uint8_t w = 8;
    OswAppWatchface::drawStepHistory(ui, (DISP_W / 2) - w * 3.5, 180, w, w * 4, OswConfigAllKeys::stepsPerDay.get(), pos);
}
#endif
void OswAppWatchface::drawWatch() {
    OswHal* hal = OswHal::getInstance();

    hal->gfx()->drawMinuteTicks(120, 120, 116, 112, ui->getForegroundDimmedColor());
    hal->gfx()->drawHourTicks(120, 120, 117, 107, ui->getForegroundColor());

#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
    uint32_t steps = hal->environment->getStepsToday();
    uint32_t stepsTarget = OswConfigAllKeys::stepsPerDay.get();
    hal->gfx()->drawArc(120, 120, 0, 360.0 * (float)(steps % stepsTarget) / (float)stepsTarget, 90, 93, 6,
                        steps > stepsTarget ? ui->getSuccessColor() : ui->getInfoColor(), true);
#endif
#ifdef OSW_FEATURE_STATS_STEPS
    OswAppWatchface::drawStepsFrame(ui,pos);
#endif
    // below two arcs take too long to draw

    // hal->gfx()->drawArc(120, 120, 0, 360, 180, 75, 7, changeColor(COLOR_GREEN, 0.25));
    // hal->gfx()->drawArc(120, 120, 0, (steps / 360) % 360, 180, 75, 7, dimColor(COLOR_GREEN, 25));
    // hal->gfx()->drawArc(120, 120, 0, (steps / 360) % 360, 180, 75, 6, COLOR_GREEN);

    // float bat = hal->getBatteryPercent() * 3.6;

    // hal->gfx()->drawArc(120, 120, 0, 360, 180, 57, 7, changeColor(COLOR_BLUE, 0.25));
    // hal->gfx()->drawArc(120, 120, 0, bat, 180, 57, 7, dimColor(COLOR_BLUE, 25));
    // hal->gfx()->drawArc(120, 120, 0, bat, 180, 57, 6, COLOR_BLUE);

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
    hal->gfx()->drawThickTick(120, 120, 16, 105, 360.0 / 60.0 * (1.0 * minute + second / 60.0), 4, ui->getForegroundColor());

#ifndef GIF_BG
    // seconds
    hal->gfx()->fillCircle(120, 120, 3, ui->getDangerColor());
    hal->gfx()->drawThickTick(120, 120, 0, 16, 180 + ( 360.0 / 60.0 * second ), 1, ui->getDangerColor());
    hal->gfx()->drawThickTick(120, 120, 0, 110, 360.0 / 60.0 * second, 1, ui->getDangerColor());
#endif
}
void OswAppWatchface::settingBrightness() {
    OswHal *hal = OswHal::getInstance();
    if (hal->btnHasGoneDown(BUTTON_3)) {
        hal->increaseBrightness(25);
    }
    if (hal->btnHasGoneDown(BUTTON_2)) {
        hal->decreaseBrightness(25);
    }
}

#ifdef GIF_BG
OswAppGifPlayer* bgGif = new OswAppGifPlayer();
#endif

void OswAppWatchface::setup() {
#ifdef GIF_BG
    bgGif->setup(hal);
#endif
#ifdef ANIMATION
    // create new animation object adapted for OSW screen
    this->matrix = new AnimMatrix(OswHal::getInstance()->gfx(), "GATC", 4, 16, 2);
#endif
}
void OswAppWatchface::loop() {
    OswHal* hal = OswHal::getInstance();
    if (hal->btnIsDown(BUTTON_3) && hal->btnHasGoneDown(BUTTON_2)) {
        buttonMode = (buttonMode + 1) % 2;
    }
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

#ifdef GIF_BG
    // if (millis() - 1000 > lastDraw) {
    bgGif->loop(hal);
    // lastDraw = millis();
    // }
#endif

#ifdef ANIMATION
    matrix->loop(hal->gfx());
#endif
    drawWatch();
    hal->requestFlush();
}

void OswAppWatchface::stop() {
#ifdef GIF_BG
    bgGif->stop();
#endif
}
