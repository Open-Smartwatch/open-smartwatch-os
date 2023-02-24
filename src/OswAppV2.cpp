#include <osw_hal.h>
#include <osw_ui.h>

#include <OswAppV2.h>
#include "assets/img/app.png.h"

OswIcon OswAppV2::defaultAppIcon = OswIcon(app_png, app_png_dimensons, 0x0); // Color will be set upon retreival

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

void OswAppV2::onStart() {
    this->updateCachedHal();
    this->needsRedraw = true;
}

void OswAppV2::onLoop() {
    this->updateCachedHal();
    const unsigned long now = millis();
    const unsigned long minPressTime = 10;
    // TODO move below values into the os configuration
    const unsigned long doublePressTimeout = 500;
    const unsigned long longPressTime = 1000;
    const unsigned long veryLongPressTime = 3000;

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
    }
}

void OswAppV2::onDraw() {
    this->updateCachedHal();
}

void OswAppV2::onStop() {
    this->updateCachedHal();
}

void OswAppV2::updateCachedHal() {
    this->hal = OswHal::getInstance();
}

void OswAppV2::onButton(int id, bool up, OswAppV2::ButtonStateNames state) {
    OSW_LOG_D("Button event for app ", this->getAppId(),": ", id, " ", up, " ", (int) state);
}

#ifdef OSW_EMULATOR
void OswAppV2::onLoopDebug() {
    // By default no debug loop (GUI) is implemented
}
#endif