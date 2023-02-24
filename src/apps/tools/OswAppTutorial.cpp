#include <gfx_util.h>
#include <OswAppV1.h>
#include <osw_hal.h>
#include <osw_ui.h>

#include "apps/tools/OswAppTutorial.h"
#include "assets/img/osw.png.h"

OswAppTutorial::OswAppTutorial(): oswIcon(osw_png, osw_png_dimensons, rgb565(200, 0, 50)), screen(0), hsv(0) {

}

const char* OswAppTutorial::getAppId() {
    return "org.osw.tutorial";
}

const char* OswAppTutorial::getAppName() {
    return "OSW Tutorial";
}

void OswAppTutorial::onStart() {
    OswAppV2::onStart(); // always make sure to call the base class method!

    this->screen = 0;
}

void OswAppTutorial::onLoop() {
    OswAppV2::onLoop(); // always make sure to call the base class method!
    this->hsv = (millis() / 100) % 255;
    
    this->needsRedraw = this->needsRedraw or this->screen == 0; // screen 0 has an animation
}

void OswAppTutorial::onDraw() {
    OswAppV2::onDraw(); // always make sure to call the base class method!

    if(this->screen == 0) {
        const unsigned char maxVal = 255;
        unsigned char r, g, b;
        hsvToRgb(this->hsv, maxVal, maxVal, r, g, b);
        this->oswIcon.color = rgb565(r, g, b);
        this->oswIcon.draw(hal->gfx(), DISP_W / 2, 32, 1.2, OswIcon::Alignment::CENTER, OswIcon::Alignment::START);
        hal->gfx()->setTextSize(2);
        hal->gfx()->setTextCenterAligned();
        hal->gfx()->setTextCursor(DISP_W / 2, 100);
        hal->gfx()->print("Welcome!");
        hal->gfx()->setTextSize(1);
        hal->gfx()->setTextCenterAligned();
        hal->gfx()->setTextCursor(DISP_W / 2, 120);
        hal->gfx()->print("This is your own Open-Smartwatch!\nIn the next few seconds this\n\"tutorial\" will guide you through\nthe basic navigation concepts\nof this OS. Have fun!\n\n\nPress any button to continue.\n\n\n");
        hal->gfx()->setTextColor(rgb565(80, 80, 80), OswUI::getInstance()->getBackgroundColor());
        hal->gfx()->print(GIT_COMMIT_HASH);
    } else if(this->screen == 1) {
        OswIcon aaa2 = OswIcon(osw_png, osw_png_dimensons, rgb565(200, 0, 50));
        aaa2.draw(hal->gfx(), DISP_W / 2, DISP_H / 2 - 32, 2, OswIcon::Alignment::CENTER, OswIcon::Alignment::CENTER);
    } else
        this->screen = 0;
}

void OswAppTutorial::onButton(int id, bool up, OswAppV2::ButtonStateNames state) {
    OswAppV2::onButton(id, up, state); // always make sure to call the base class method!
    if(this->screen == 0)
        this->screen = 1;
}

#ifdef OSW_EMULATOR
#include "imgui.h"

void OswAppTutorial::onLoopDebug() {
    ImGui::Begin("Debug: OswAppTutorial");
    ImGui::InputScalar("screen", ImGuiDataType_U8, &this->screen);
    ImGui::End();
}
#endif

void OswAppTutorial::onStop() {
    OswAppV2::onStop(); // always make sure to call the base class method!
}