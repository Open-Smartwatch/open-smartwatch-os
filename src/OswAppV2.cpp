#include <osw_hal.h>
#include <osw_ui.h>

#include <OswAppV2.h>
#include "assets/img/icons/app.png.h"

OswIcon OswAppV2::defaultAppIcon = OswIcon(app_png, app_png_dimensions, 0x0); // Color will be set upon retreival

OswAppV2::OswAppV2() {
    for(int i = 0; i < NUM_BUTTONS; i++)
        // Do not listen to the double press, as it may delays the short press reporting
        this->knownButtonStates[i] = (ButtonStateNames) (ButtonStateNames::UNDEFINED | ButtonStateNames::SHORT_PRESS | ButtonStateNames::LONG_PRESS | ButtonStateNames::VERY_LONG_PRESS);
}

OswIcon& OswAppV2::getAppIcon() {
    return this->getDefaultAppIcon();
}

OswIcon& OswAppV2::getDefaultAppIcon() {
    this->defaultAppIcon.color = OswUI::getInstance()->getPrimaryColor(); // Update the color of the default icon (because the static init is black or the color may changed)
    return this->defaultAppIcon;
}

OswAppV2::ViewFlags OswAppV2::getViewFlags() {
    return this->viewFlags;
}

bool OswAppV2::getNeedsRedraw() {
    return this->needsRedraw;
}

void OswAppV2::resetNeedsRedraw() {
    this->needsRedraw = false;
}

void OswAppV2::onStart() {
    this->needsRedraw = true;
}

void OswAppV2::onLoop() {
    const unsigned long now = millis();
    const unsigned long minPressTime = 10;
    // TODO move below values into the os configuration
    const unsigned long doublePressTimeout = 500;
    const unsigned long longPressTime = 1000;
    const unsigned long veryLongPressTime = 3000;

    const unsigned long indicatorMinTime = minPressTime + (longPressTime * 0.2);
    for(int i = 0; i < NUM_BUTTONS; i++) {
        if(hal->btnIsDownSince((Button) i) > 0) {
            if(buttonDownSince[i] == 0) {
                // Oh, the button just went down!
                buttonDownSince[i] = now;
                buttonLastSentState[i] = ButtonStateNames::UNDEFINED;
                if(this->knownButtonStates[i] & buttonLastSentState[i])
                    this->onButton(i, false, buttonLastSentState[i]); // we can't decide the short/long press, as it just happend
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
                        this->onButton(i, false, buttonLastSentState[i]);
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
            if(buttonLastSentState[i] != ButtonStateNames::UNDEFINED)
                this->onButton(i, true, buttonLastSentState[i]);
            buttonDownSince[i] = 0;
            buttonLastSentState[i] = ButtonStateNames::UNDEFINED;
        }
        if(hal->btnIsDownSince((Button) i) == 0 and this->knownButtonStates[i] & ButtonStateNames::SHORT_PRESS) // If the button is not down, check if the double press timeout is over
            if(buttonDoubleShortTimeout[i] > 0 and now - buttonDoubleShortTimeout[i] >= doublePressTimeout) {
                buttonDoubleShortTimeout[i] = 0; // Reset the double press timeout (if it is set and send out the short press event as no double press happend in time)
                this->onButton(i, true, ButtonStateNames::SHORT_PRESS);
            }

        // Now update the indicator-levels for the buttons
        if(hal->btnIsDownFor((Button) i) > indicatorMinTime) {
            float progress = (hal->btnIsDownFor((Button) i) - indicatorMinTime) / (float) longPressTime;
            if(progress > 1.0) {
                // Oh! The button is down for a very long time!
                progress = 1.0 + (hal->btnIsDownFor((Button) i) - indicatorMinTime - longPressTime) / (float) (veryLongPressTime - longPressTime);
                if(progress > 2.0)
                    progress = 2.0;
            }
            if(this->buttonIndicatorProgress[i] != progress) {
                this->buttonIndicatorProgress[i] = progress;
                this->needsRedraw = true;
            }
        } else if(this->buttonIndicatorProgress[i] != 0.0) {
            this->buttonIndicatorProgress[i] = 0.0;
            this->needsRedraw = true;
        }
    }
}

void OswAppV2::onDraw() {

}

void OswAppV2::onDrawOverlay() {
    // IDEA for monochrome displays: Just draw a (in length) increasing circle-arc, for very long fill it completely
    for(int i = 0; i < NUM_BUTTONS; i++) {
        if(this->buttonIndicatorProgress[i] == 0.0)
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
        const int16_t veryLongRad = 12; 
        if(this->buttonIndicatorProgress[i] <= 1.0)
            hal->gfx()->fillCircle(btnX, btnY, this->buttonIndicatorProgress[i] * longRad, OswUI::getInstance()->getPrimaryColor());
        else {
            float overcut = 0.2;
            float secondRad = (1.0 + overcut) * veryLongRad;
            hal->gfx()->fillCircle(btnX, btnY, (1.0 - overcut) * longRad + (this->buttonIndicatorProgress[i] - 1.0) * secondRad, OswUI::getInstance()->getWarningColor());

            hal->gfx()->fillCircle(btnX, btnY, longRad, OswUI::getInstance()->getPrimaryColor());
        }
    }
}

void OswAppV2::onStop() {

}

void OswAppV2::onButton(int id, bool up, OswAppV2::ButtonStateNames state) {
    OSW_LOG_D("Button event for app ", this->getAppId(),": id? ", id, " up? ", up, " state? ", (int) state);
}

#ifdef OSW_EMULATOR
void OswAppV2::onLoopDebug() {
    // By default no debug loop (GUI) is implemented
}
#endif