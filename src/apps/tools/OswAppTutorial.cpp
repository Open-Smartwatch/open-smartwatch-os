#include <gfx_util.h>
#include <OswAppV1.h>
#include <osw_hal.h>
#include <osw_ui.h>

#include "apps/tools/OswAppTutorial.h"
#include "assets/img/osw.png.h"
#include "assets/img/wait.png.h"
#include "assets/img/check.png.h"

OswAppTutorial::OswAppTutorial(): oswIcon(osw_png, osw_png_dimensons, rgb565(200, 0, 50)) {

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
        OswIcon waiting = OswIcon(wait_png, wait_png_dimensons, rgb565(200, 0, 50));
        OswIcon checked = OswIcon(check_png, check_png_dimensons, rgb565(0, 200, 50));
        hal->gfx()->setTextSize(2);
        hal->gfx()->setTextCenterAligned();
        hal->gfx()->setTextCursor(DISP_W / 2, 100);
        hal->gfx()->print("Navigation");
        hal->gfx()->setTextSize(1);
        hal->gfx()->setTextCenterAligned();
        hal->gfx()->setTextCursor(DISP_W / 2, 120);
        hal->gfx()->print("Please press the button modes\nlisted below to continue.");

        hal->gfx()->setTextSize(1);
        hal->gfx()->setTextLeftAligned();

        short y = 160;
        hal->gfx()->setTextCursor(80, y);
        hal->gfx()->print("Short Press");
        if(this->gotButtonShort)
            checked.draw(hal->gfx(), 80 - 5, y - 3, 1, OswIcon::Alignment::END, OswIcon::Alignment::CENTER);
        else
            waiting.draw(hal->gfx(), 80 - 5, y - 3, 1, OswIcon::Alignment::END, OswIcon::Alignment::CENTER);
        y += 15;
        hal->gfx()->setTextCursor(80, y);
        hal->gfx()->print("Long Press");
        if(this->gotButtonLong)
            checked.draw(hal->gfx(), 80 - 5, y - 3, 1, OswIcon::Alignment::END, OswIcon::Alignment::CENTER);
        else
            waiting.draw(hal->gfx(), 80 - 5, y - 3, 1, OswIcon::Alignment::END, OswIcon::Alignment::CENTER);
        y += 15;
        hal->gfx()->setTextCursor(80, y);
        hal->gfx()->print("Very Long Press");
        if(this->gotButtonVeryLong)
            checked.draw(hal->gfx(), 80 - 5, y - 3, 1, OswIcon::Alignment::END, OswIcon::Alignment::CENTER);
        else
            waiting.draw(hal->gfx(), 80 - 5, y - 3, 1, OswIcon::Alignment::END, OswIcon::Alignment::CENTER);
        y += 15;
        hal->gfx()->setTextCursor(80, y);
        hal->gfx()->print("Double Press");
        if(this->gotButtonDouble)
            checked.draw(hal->gfx(), 80 - 5, y - 3, 1, OswIcon::Alignment::END, OswIcon::Alignment::CENTER);
        else
            waiting.draw(hal->gfx(), 80 - 5, y - 3, 1, OswIcon::Alignment::END, OswIcon::Alignment::CENTER);
    } else
        this->screen = 0;
}

void OswAppTutorial::onButton(int id, bool up, OswAppV2::ButtonStateNames state) {
    OswAppV2::onButton(id, up, state); // always make sure to call the base class method!
    if(!up) return;
    if(this->screen == 0) {
        this->screen = 1;
        this->needsRedraw = true;
        // Also enable double press detection
        for(int i = 0; i < NUM_BUTTONS; i++)
            this->knownButtonStates[i] = (OswAppV2::ButtonStateNames) (this->knownButtonStates[i] | OswAppV2::ButtonStateNames::DOUBLE_PRESS);
    } else if(this->screen == 1) {
        if(state == OswAppV2::ButtonStateNames::SHORT_PRESS)
            this->gotButtonShort = true;
        else if(state == OswAppV2::ButtonStateNames::LONG_PRESS)
            this->gotButtonLong = true;
        else if(state == OswAppV2::ButtonStateNames::VERY_LONG_PRESS)
            this->gotButtonVeryLong = true;
        else if(state == OswAppV2::ButtonStateNames::DOUBLE_PRESS)
            this->gotButtonDouble = true;
        if(this->gotButtonShort and this->gotButtonLong and this->gotButtonVeryLong and this->gotButtonDouble) {
            this->screen = 2;
            // Disable double press detection again (speeds up short press detection again)
            for(int i = 0; i < NUM_BUTTONS; i++)
                this->knownButtonStates[i] = (OswAppV2::ButtonStateNames) (this->knownButtonStates[i] ^ OswAppV2::ButtonStateNames::DOUBLE_PRESS); // Using XOR, as we know it was enabled before
        }
        this->needsRedraw = true;
    }
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