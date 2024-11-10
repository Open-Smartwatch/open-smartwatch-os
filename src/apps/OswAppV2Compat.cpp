#include <apps/OswAppV2Compat.h>

OswAppV2Compat::OswAppV2Compat(const char* id, const char* name, OswAppV1& app, bool keepScreenOn, std::optional<std::reference_wrapper<const OswIcon>> icon): id(id), name(name), icon(icon.value_or(this->getDefaultAppIcon())), app(app), keepScreenOn(keepScreenOn) {

}

const char* OswAppV2Compat::getAppId() {
    return this->id;
}

const char* OswAppV2Compat::getAppName() {
    return this->name;
}

const OswIcon& OswAppV2Compat::getAppIcon() {
    return this->icon;
}

void OswAppV2Compat::onStart() {
    this->viewFlags = (OswAppV2::ViewFlags)(this->viewFlags | OswAppV2::ViewFlags::NO_FPS_LIMIT | (this->keepScreenOn ? OswAppV2::ViewFlags::KEEP_DISPLAY_ON : OswAppV2::ViewFlags::NONE)); // this causes draw() to be called upon every loop() call -> preventing the loss of input events (like the button just went down)
    // Listen to ALL the typical button events (while not processed, it allows animations to be shown for them)
    for(int i = 0; i < BTN_NUMBER; i++)
        this->knownButtonStates[i] = (ButtonStateNames) (ButtonStateNames::UNDEFINED | ButtonStateNames::SHORT_PRESS | ButtonStateNames::LONG_PRESS | ButtonStateNames::VERY_LONG_PRESS);
    this->app.setup();
}

void OswAppV2Compat::onLoop() {
    this->needsRedraw = true;
}

void OswAppV2Compat::onDraw() {
    this->app.loop(); // v1 apps draw in their loop, so we call loop here (which may causes rendering slowdown)
}

void OswAppV2Compat::onDrawOverlay() {
    // Unsupported.
}

void OswAppV2Compat::onStop() {
    this->app.stop();
}

#ifdef OSW_EMULATOR
void OswAppV2Compat::onLoopDebug() {
    this->app.loopDebug();
}
#endif
