#include <osw_hal.h>
#include <osw_ui.h>
#include <osw_config_keys.h>

#include <OswAppV2.h>
#include "assets/img/icons/app.png.h"

OswIconProgmem OswAppV2::defaultAppIcon = app_png; // Copy, so color can be set upon retreival

OswAppV2::OswAppV2() {

}

const OswIcon& OswAppV2::getAppIcon() {
    return this->getDefaultAppIcon();
}

const OswIcon& OswAppV2::getDefaultAppIcon() {
    this->defaultAppIcon.color = OswUI::getInstance()->getPrimaryColor(); // Update the color of the default icon (because the color may changed)
    return this->defaultAppIcon;
}

const OswAppV2::ViewFlags& OswAppV2::getViewFlags() {
    return this->viewFlags;
}

bool OswAppV2::getNeedsRedraw() {
    return this->needsRedraw;
}

void OswAppV2::resetNeedsRedraw() {
    this->needsRedraw = false;
}

void OswAppV2::onStart() {
#ifndef NDEBUG
    // Can't run this during startup, as this method should be overriden by the app
    if(String(this->getAppId()).length() > 15)
        OSW_LOG_W("The app id ", this->getAppId(), " is longer than 15 characters. This may cause problems with some api calls (e.g. Preferences).");
#endif

    this->needsRedraw = true;
    this->clearKnownButtonStates();
}

void OswAppV2::clearKnownButtonStates() {
    for(int i = 0; i < BTN_NUMBER; i++)
        this->knownButtonStates[i] = (ButtonStateNames) 0; // By default no buttons are supported
}

void OswAppV2::onLoop() {
    const unsigned long now = millis();
    const unsigned short minPressTime = 10;
    const unsigned short doublePressTimeout = OswConfigAllKeys::oswAppV2ButtonDoublePress.get();
    const unsigned short longPressTime = OswConfigAllKeys::oswAppV2ButtonLongPress.get();
    const unsigned short veryLongPressTime = OswConfigAllKeys::oswAppV2ButtonVeryLongPress.get();

    const unsigned long indicatorMinTime = minPressTime + (longPressTime * 0.2f);
    for(char i = 0; i < BTN_NUMBER; i++) {
        if(hal->btnIsDownSince((Button) i) > 0) {
            if(buttonDownSince[i] == 0) {
                // Oh, the button just went down!
                buttonDownSince[i] = now;
                buttonLastSentState[i] = ButtonStateNames::UNDEFINED;
                if(this->knownButtonStates[i] & buttonLastSentState[i])
                    this->onButton((Button) i, false, buttonLastSentState[i]); // we can't decide the short/long press, as it just happend
            } else {
                // Send state updates, while the button is down
                ButtonStateNames maybeNewState = ButtonStateNames::UNDEFINED;
                if(now - buttonDownSince[i] >= minPressTime and now - buttonDownSince[i] < longPressTime) {
                    maybeNewState = ButtonStateNames::SHORT_PRESS;
                } else if(now - buttonDownSince[i] >= longPressTime && now - buttonDownSince[i] < veryLongPressTime) {
                    maybeNewState = ButtonStateNames::LONG_PRESS;
                } else if(now - buttonDownSince[i] >= veryLongPressTime) {
                    maybeNewState = ButtonStateNames::VERY_LONG_PRESS;
                }
                if(maybeNewState != buttonLastSentState[i]) {
                    buttonLastSentState[i] = maybeNewState;
                    if(this->knownButtonStates[i] & buttonLastSentState[i])
                        this->onButton((Button) i, false, buttonLastSentState[i]);
                }
            }
        } else if(buttonDownSince[i] > 0) {
            // Oh, the button just went up!
            if(this->knownButtonStates[i] & ButtonStateNames::DOUBLE_PRESS and buttonLastSentState[i] == ButtonStateNames::SHORT_PRESS) {
                if(buttonDoubleShortTimeout[i] > 0 and now - buttonDoubleShortTimeout[i] < doublePressTimeout) {
                    buttonLastSentState[i] = ButtonStateNames::DOUBLE_PRESS;
                    buttonDoubleShortTimeout[i] = 0;
                } else {
                    // Do not send the short press event, as it may be a double press
                    buttonLastSentState[i] = ButtonStateNames::UNDEFINED;
                    buttonDoubleShortTimeout[i] = now;
                }
            } else
                buttonDoubleShortTimeout[i] = 0; // Reset the double press timeout on any other button state
            // Special case handling: If the user pressed very long, but the app does not support very long presses, send out the long press event
            if(this->knownButtonStates[i] & ButtonStateNames::LONG_PRESS and !(this->knownButtonStates[i] & ButtonStateNames::VERY_LONG_PRESS) and buttonLastSentState[i] == ButtonStateNames::VERY_LONG_PRESS)
                buttonLastSentState[i] = ButtonStateNames::LONG_PRESS;
            if(buttonLastSentState[i] != ButtonStateNames::UNDEFINED and this->knownButtonStates[i] & buttonLastSentState[i])
                this->onButton((Button) i, true, buttonLastSentState[i]);
            buttonDownSince[i] = 0;
            buttonLastSentState[i] = ButtonStateNames::UNDEFINED;
        }
        if(hal->btnIsDownSince((Button) i) == 0 and this->knownButtonStates[i] & ButtonStateNames::SHORT_PRESS) // If the button is not down, check if the double press timeout is over
            if(buttonDoubleShortTimeout[i] > 0 and now - buttonDoubleShortTimeout[i] >= doublePressTimeout) {
                buttonDoubleShortTimeout[i] = 0; // Reset the double press timeout (if it is set and send out the short press event as no double press happend in time)
                this->onButton((Button) i, true, ButtonStateNames::SHORT_PRESS);
            }

        // Now update the indicator-levels for the buttons
        if(hal->btnIsDownFor((Button) i) > indicatorMinTime) {
            float progress = (hal->btnIsDownFor((Button) i) - indicatorMinTime) / (float) longPressTime;
            if(progress > 1.0f) {
                // Oh! The button is down for a very long time!
                progress = 1.0f + (hal->btnIsDownFor((Button) i) - indicatorMinTime - longPressTime) / (float) (veryLongPressTime - longPressTime);
                if(progress > 2.0f)
                    progress = 2.0f;
            }
            if(this->buttonIndicatorProgress[i] != progress) {
                this->buttonIndicatorProgress[i] = progress;
                if(this->knownButtonStates[i] & ButtonStateNames::LONG_PRESS or
                        this->knownButtonStates[i] & ButtonStateNames::VERY_LONG_PRESS) {
                    // Only redraw if any of the animation need to be drawn
                    this->needsRedraw = true;
                }
            }
        } else if(this->buttonIndicatorProgress[i] != 0.0f) {
            this->buttonIndicatorProgress[i] = 0.0f;
            this->needsRedraw = true;
        }
    }
}

void OswAppV2::onDraw() {

}

void OswAppV2::onDrawOverlay() {
    // IDEA for monochrome displays: Just draw a (in length) increasing circle-arc, for very long fill it completely
    for(int i = 0; i < BTN_NUMBER; i++) {
        // Is the button supported?
        if(this->knownButtonStates[i] == 0)
            continue;
        // If the button is not down, skip it
        if(this->buttonIndicatorProgress[i] == 0.0f)
            continue;

        int16_t btnX = 0;
        int16_t btnY = 0;
        hal->getButtonCoordinates((Button) i, btnX, btnY);
        int16_t btnOffset = 4;
        if(btnX < DISP_W / 2)
            btnX -= btnOffset;
        else
            btnX += btnOffset;
        if(btnY < DISP_H / 2)
            btnY -= btnOffset;
        else
            btnY += btnOffset;

        const int16_t longRad = 24;
        const int16_t veryLongRad = 14;
        if(this->knownButtonStates[i] & ButtonStateNames::VERY_LONG_PRESS and this->buttonIndicatorProgress[i] > 1.0f) {
            // Very long-press circle
            const float overcut = 0.2f;
            const float secondRad = (1.0f + overcut) * veryLongRad;
            hal->gfx()->fillCircle(btnX, btnY, (1.0f - overcut) * longRad + (this->buttonIndicatorProgress[i] - 1.0f) * secondRad, OswUI::getInstance()->getWarningColor());
        }
        if(this->knownButtonStates[i] & ButtonStateNames::LONG_PRESS) {
            // Long-press circle
            hal->gfx()->fillCircle(btnX, btnY, min((int16_t) (this->buttonIndicatorProgress[i] * longRad), longRad), OswUI::getInstance()->getPrimaryColor());
        }
    }
}

void OswAppV2::onStop() {

}

void OswAppV2::onButton(Button id, bool up, OswAppV2::ButtonStateNames state) {
    OSW_LOG_D("Button event for app ", this->getAppId(),": id? ", id, " up? ", up, " state? ", (int) state);
}

#ifdef OSW_EMULATOR
void OswAppV2::onLoopDebug() {
    // By default no debug loop (GUI) is implemented
}
#endif