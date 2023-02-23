#include <osw_hal.h>

#include <OswAppV2.h>

OswIcon OswAppV2::defaultAppIcon = OswIcon(nullptr, rgb565(100, 200, 50)); // TODO

OswIcon& OswAppV2::getAppIcon() {
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
    // TODO handle button changes and pass them to onButton
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

void OswAppV2::onButton(int id, bool down, bool shortPress, bool longPress) {
    OSW_LOG_W("Not inplemented yet!");
}

#ifdef OSW_EMULATOR
void OswAppV2::onLoopDebug() {
    // By default no debug loop (GUI) is implemented
}
#endif