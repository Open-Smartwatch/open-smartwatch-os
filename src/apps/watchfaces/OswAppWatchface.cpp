#include <gfx_util.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>
#include "globals.h"

#ifdef ANIMATION
#include <animations/anim_matrix.h>
#endif
#ifdef GIF_BG
#include "./apps/_experiments/gif_player.h"
#endif

#include "./apps/watchfaces/OswAppWatchface.h"

#define CENTER_X (DISP_W / 2)
#define CENTER_Y (DISP_H / 2)

const char* OswAppWatchface::getAppId() {
    return OswAppWatchface::APP_ID;
}

const char* OswAppWatchface::getAppName() {
    return LANG_ANALOG;
}

#ifdef OSW_FEATURE_STATS_STEPS
void OswAppWatchface::drawStepHistory(OswUI* ui, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t max) {
    OswHal* hal = OswHal::getInstance();
    OswUI::getInstance()->resetTextColors();

    OswDate oswDate = { };
    hal->getLocalDate(oswDate);

    for (uint8_t i = 0; i < 7; i++) {
        uint32_t s = hal->environment()->getStepsOnDay(i);
        uint16_t boxHeight = ((float)(s > max ? max : s) / max) * h;
        boxHeight = boxHeight < 2 ? 0 : boxHeight;

        // step bars
        uint16_t c = (unsigned int) OswConfigAllKeys::stepsPerDay.get() <= s ? ui->getSuccessColor() : ui->getPrimaryColor();
        hal->gfx()->fillFrame(x + i * w, y + (h - boxHeight), w, boxHeight, c);
        // bar frames
        uint16_t f = oswDate.weekDay == i ? ui->getForegroundColor() : ui->getForegroundDimmedColor();
        hal->gfx()->drawRFrame(x + i * w, y, w, h, 2, f);

        // labels
        hal->gfx()->setTextCenterAligned();  // horiz.
        hal->gfx()->setTextBottomAligned();
        hal->gfx()->setTextSize(1);
        hal->gfx()->setTextCursor(CENTER_X, y - 1);

        hal->gfx()->print(hal->environment()->getStepsToday() + (OswConfigAllKeys::settingDisplayStepsGoal.get() ? String("/") + max:""));

        hal->gfx()->setTextCursor(CENTER_X, y + 1 + 8 + w * 4);
        hal->gfx()->setTextColor(ui->getForegroundColor());  // Let's make the background transparent.
        // See : https://github.com/Open-Smartwatch/open-smartwatch-os/issues/194
        // font : WHITE / bg : None
        hal->gfx()->print(hal->environment()->getStepsTotal());
        hal->gfx()->setTextColor(ui->getForegroundColor(), ui->getBackgroundColor());  // restore. font : WHITE / bg : BLACK
    }
}
#endif

void OswAppWatchface::drawWatch() {
    OswHal* hal = OswHal::getInstance();


    hal->gfx()->drawMinuteTicks(CENTER_Y, CENTER_Y, 116, 112, ui->getForegroundDimmedColor(), true);
    hal->gfx()->drawHourTicks(CENTER_X, CENTER_Y, 117, 107, ui->getForegroundColor(), true);

#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
    uint32_t steps = hal->environment()->getStepsToday();
    uint32_t stepsTarget = OswConfigAllKeys::stepsPerDay.get();
    hal->gfx()->drawArc(CENTER_X, CENTER_Y, 0, 360.0f * (float)(steps % stepsTarget) / (float)stepsTarget, 90, 93, 6,
                        steps > stepsTarget ? ui->getSuccessColor() : ui->getInfoColor(), true);
#endif

#ifdef OSW_FEATURE_STATS_STEPS
    uint8_t w = 8;
    OswAppWatchface::drawStepHistory(ui, (CENTER_X) - w * 3.5f, 180, w, w * 4, OswConfigAllKeys::stepsPerDay.get());
#endif

    // below two arcs take too long to draw

    // hal->gfx()->drawArc(120, 120, 0, 360, 180, 75, 7, changeColor(COLOR_GREEN, 0.25f));
    // hal->gfx()->drawArc(120, 120, 0, (steps / 360) % 360, 180, 75, 7, dimColor(COLOR_GREEN, 25));
    // hal->gfx()->drawArc(120, 120, 0, (steps / 360) % 360, 180, 75, 6, COLOR_GREEN);

    // float bat = hal->getBatteryPercent() * 3.6f;

    // hal->gfx()->drawArc(120, 120, 0, 360, 180, 57, 7, changeColor(COLOR_BLUE, 0.25f));
    // hal->gfx()->drawArc(120, 120, 0, bat, 180, 57, 7, dimColor(COLOR_BLUE, 25));
    // hal->gfx()->drawArc(120, 120, 0, bat, 180, 57, 6, COLOR_BLUE);

    OswTime oswTime = { };
    hal->getLocalTime(oswTime);

    if(OswConfigAllKeys::settingDisplayDualHourTick.get()) {
        OswTime oswDualTime = { };
        hal->getDualTime(oswDualTime);

        // dual-hours
        hal->gfx()->drawThickTick(CENTER_X, CENTER_Y, 0, 16, (int)(360.0f / 12.0f * (oswDualTime.hour + oswDualTime.minute / 60.0f)), 2, ui->getBackgroundDimmedColor(), true, STRAIGHT_END);
        hal->gfx()->drawThickTick(CENTER_X, CENTER_Y, 16, 60, (int)(360.0f / 12.0f * (oswDualTime.hour + oswDualTime.minute / 60.0f)), 5, ui->getBackgroundDimmedColor(), true);
    }
    // hours
    hal->gfx()->drawThickTick(CENTER_X, CENTER_Y,  0, (int)16, (int)(360.0f / 12.0f * (oswTime.hour + oswTime.minute / 60.0f)), 1, ui->getForegroundColor(), true, STRAIGHT_END);
    hal->gfx()->drawThickTick(CENTER_X, CENTER_Y, 16, 60, (int)(360.0f / 12.0f * (oswTime.hour + oswTime.minute / 60.0f)), 4, ui->getForegroundColor(), true);

    // minutes
    hal->gfx()->drawThickTick(CENTER_X, CENTER_Y, 0, 16, (int)(360.0f / 60.0f * (oswTime.minute + oswTime.second / 60.0f)), 1, ui->getForegroundColor(), true, STRAIGHT_END);
    hal->gfx()->drawThickTick(CENTER_X, CENTER_Y, 16, 105, (int)(360.0f / 60.0f * (oswTime.minute + oswTime.second / 60.0f)), 4, ui->getForegroundColor(), true);

#ifndef GIF_BG
    // seconds
    hal->gfx()->fillCircleAA(CENTER_X, CENTER_Y, 3, ui->getDangerColor());
    hal->gfx()->drawThickTick(CENTER_X, CENTER_Y, -16, 110, 180 + 360 / 60 * oswTime.second, 1, ui->getDangerColor(), true);
#endif
}

void OswAppWatchface::onStart() {
    OswAppV2::onStart();
    this->viewFlags = OswAppV2::ViewFlags::NO_OVERLAYS; // no overlay for this watchface
    OswAppWatchface::addButtonDefaults(this->knownButtonStates);
#ifdef GIF_BG
    this->bgGif = new OswAppGifPlayer();
    this->bgGif->setup();
#endif
#ifdef ANIMATION
    // create new animation object adapted for OSW screen
    this->matrix = new AnimMatrix(OswHal::getInstance()->gfx(), "GATC", 4, 16, 2);
#endif
}

void OswAppWatchface::onLoop() {
    OswAppV2::onLoop();

    this->needsRedraw = this->needsRedraw or time(nullptr) != this->lastTime; // redraw every second
}

void OswAppWatchface::onDraw() {
    OswAppV2::onDraw();

#ifdef GIF_BG
    if(this->bgGif != nullptr)
        this->bgGif->loop();
#endif

#ifdef ANIMATION
    matrix->loop(OswHal::getInstance()->gfx());
#endif
    drawWatch();

    this->lastTime = time(nullptr);
}

void OswAppWatchface::onButton(Button id, bool up, OswAppV2::ButtonStateNames state) {
    OswAppV2::onButton(id, up, state);
    if(OswAppWatchface::onButtonDefaults(*this, id, up, state))
        return; // if the button was handled by the defaults, we are done here
}

void OswAppWatchface::addButtonDefaults(std::array<OswAppV2::ButtonStateNames, BTN_NUMBER>& knownButtonStates) {
    knownButtonStates[Button::BUTTON_UP] = OswAppV2::ButtonStateNames::SHORT_PRESS;
    knownButtonStates[Button::BUTTON_DOWN] = (OswAppV2::ButtonStateNames) (OswAppV2::ButtonStateNames::SHORT_PRESS | OswAppV2::ButtonStateNames::LONG_PRESS);
}

bool OswAppWatchface::onButtonDefaults(OswAppV2& app, Button id, bool up, OswAppV2::ButtonStateNames state) {
    if(!up) return false;
    if(state == OswAppV2::ButtonStateNames::SHORT_PRESS) {
        if(id == Button::BUTTON_UP) {
            OswHal::getInstance()->increaseBrightness(25);
            return true;
        } else if(id == Button::BUTTON_DOWN) {
            OswHal::getInstance()->decreaseBrightness(25);
            return true;
        }
    } else if(state == OswAppV2::ButtonStateNames::LONG_PRESS and id == Button::BUTTON_DOWN and OswConfigAllKeys::settingDisplayDefaultWatchface.get() != app.getAppId()) {
        OSW_LOG_I("Setting default watchface to: ", app.getAppId());
        OswConfig::getInstance()->enableWrite();
        try {
            OswConfigAllKeys::settingDisplayDefaultWatchface.set(app.getAppId()); // if this app-id is not part of the list this may throw an exception
        } catch(const std::invalid_argument& e) {
            OSW_LOG_E("Failed to set default watchface: ", e.what());
        }
        OswConfig::getInstance()->disableWrite();
        return true;
    }
    return false;
}

void OswAppWatchface::onStop() {
    OswAppV2::onStop();
#ifdef GIF_BG
    if(this->bgGif != nullptr) {
        this->bgGif->stop();
        delete this->bgGif;
        this->bgGif = nullptr;
    }
#endif
}
