#include <OswAppV2Compat.h>

OswAppV2Compat::OswAppV2Compat(const char* id, const char* name, OswAppV1& app): id(id), name(name), app(app) {

}

const char* OswAppV2Compat::getAppId() {
    return this->id;
}

const char* OswAppV2Compat::getAppName() {
    return this->name;
}

void OswAppV2Compat::onStart() {
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