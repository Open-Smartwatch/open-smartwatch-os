#include <osw_hal.h>
#include <osw_ui.h>

#include <OswAppV2.h>
#include "assets/img/app.png.h"

OswIcon OswAppV2::defaultAppIcon = OswIcon(app_png, app_png_dimensons, 0x0); // Color will be set upon retreival

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