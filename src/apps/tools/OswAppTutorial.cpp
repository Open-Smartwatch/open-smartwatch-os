#include <gfx_util.h>
#include <OswAppV1.h>
#include <osw_hal.h>
#include <osw_ui.h>
#include <Preferences.h>

#include "apps/tools/OswAppTutorial.h"
#include "assets/img/icons/osw.png.h"
#include "assets/img/icons/wait.png.h"
#include "assets/img/icons/check.png.h"
#include "assets/img/icons/warning.png.h"

OswAppTutorial::OswAppTutorial(): oswIcon(osw_png, osw_png_dimensions, rgb565(200, 0, 50)) {
    bool res = nvs.begin(this->getAppId(), false);
    assert(res);
}

OswAppTutorial::~OswAppTutorial() {
    nvs.end();
}

const char* OswAppTutorial::getAppId() {
    return "osw.tutorial";
}

const char* OswAppTutorial::getAppName() {
    return "OSW Tutorial";
}

void OswAppTutorial::onStart() {
    OswAppV2::onStart(); // always make sure to call the base class method!

    this->screen = 0;
    this->timeout = time(nullptr);
}

void OswAppTutorial::onLoop() {
    OswAppV2::onLoop(); // always make sure to call the base class method!
    this->hsv = (millis() / 100) % 255;
    
    this->needsRedraw = this->needsRedraw or this->screen == 0 or this->currentScreen != this->screen; // screen 0 has an animation
    this->currentScreen = this->screen;
}

void OswAppTutorial::onDraw() {
    OswAppV2::onDraw(); // always make sure to call the base class method!

    if(this->screen == 0) {
        const unsigned char maxVal = 255;
        unsigned char r, g, b;
        hsvToRgb(this->hsv, maxVal, maxVal, r, g, b);
        this->oswIcon.color = rgb565(r, g, b);
        this->oswIcon.draw(hal->gfx(), DISP_W / 2, 28, 3, OswImage::Alignment::CENTER, OswImage::Alignment::START);
        hal->gfx()->setTextSize(2);
        hal->gfx()->setTextCenterAligned();
        hal->gfx()->setTextCursor(DISP_W / 2, 100);
        hal->gfx()->print("Welcome!");
        hal->gfx()->setTextSize(1);
        hal->gfx()->setTextCenterAligned();
        hal->gfx()->setTextCursor(DISP_W / 2, 120);
        hal->gfx()->print("This is your own Open-Smartwatch!\nIn the next few seconds this\n\"tutorial\" will guide you through\nthe basic navigation concepts\nof this OS. Have fun!");
        hal->gfx()->setTextCursor(DISP_W / 2, 180);
        hal->gfx()->print("Press any button to continue.\n\n\n");
        hal->gfx()->setTextColor(rgb565(80, 80, 80), OswUI::getInstance()->getBackgroundColor());
        hal->gfx()->print(GIT_COMMIT_HASH);
    } else if(this->screen == 1) {
        OswIcon waiting = OswIcon(wait_png, wait_png_dimensions, rgb565(200, 0, 50));
        OswIcon checked = OswIcon(check_png, check_png_dimensions, rgb565(0, 200, 50));
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
            checked.draw(hal->gfx(), 80 - 5, y - 3, 1, OswImage::Alignment::END, OswImage::Alignment::CENTER);
        else
            waiting.draw(hal->gfx(), 80 - 5, y - 3, 1, OswImage::Alignment::END, OswImage::Alignment::CENTER);
        y += 15;
        hal->gfx()->setTextCursor(80, y);
        hal->gfx()->print("Long Press");
        if(this->gotButtonLong)
            checked.draw(hal->gfx(), 80 - 5, y - 3, 1, OswImage::Alignment::END, OswImage::Alignment::CENTER);
        else
            waiting.draw(hal->gfx(), 80 - 5, y - 3, 1, OswImage::Alignment::END, OswImage::Alignment::CENTER);
        y += 15;
        hal->gfx()->setTextCursor(80, y);
        hal->gfx()->print("Very Long Press");
        if(this->gotButtonVeryLong)
            checked.draw(hal->gfx(), 80 - 5, y - 3, 1, OswImage::Alignment::END, OswImage::Alignment::CENTER);
        else
            waiting.draw(hal->gfx(), 80 - 5, y - 3, 1, OswImage::Alignment::END, OswImage::Alignment::CENTER);
        y += 15;
        hal->gfx()->setTextCursor(80, y);
        hal->gfx()->print("Double Press");
        if(this->gotButtonDouble)
            checked.draw(hal->gfx(), 80 - 5, y - 3, 1, OswImage::Alignment::END, OswImage::Alignment::CENTER);
        else
            waiting.draw(hal->gfx(), 80 - 5, y - 3, 1, OswImage::Alignment::END, OswImage::Alignment::CENTER);
    } else if(this->screen == 2) {
        hal->gfx()->setTextSize(2);
        hal->gfx()->setTextCenterAligned();
        hal->gfx()->setTextCursor(DISP_W / 2, 100);
        hal->gfx()->print("Battery Calibration");
        hal->gfx()->setTextSize(1);
        hal->gfx()->setTextCenterAligned();
        hal->gfx()->setTextCursor(DISP_W / 2, 120);
        hal->gfx()->print("As this hardware has no BMS,\nthe OS has to learn the battery\ncapacity on-the-fly. Make sure to\nfully discharge the battery if\nyou see the battery icon being");
        hal->gfx()->setTextColor(OswUI::getInstance()->getInfoColor(), OswUI::getInstance()->getBackgroundColor());
        hal->gfx()->print("filled with the \"info\" color.");
        OswUI::getInstance()->resetTextColors();
        hal->gfx()->setTextCursor(DISP_W / 2, 180);
        hal->gfx()->print("Press any button to continue.");
    } else if(this->screen == 3) {
        OswIcon warning = OswIcon(warning_png, warning_png_dimensions, OswUI::getInstance()->getWarningColor());
        warning.draw(hal->gfx(), DISP_W / 2, 28, 3, OswImage::Alignment::CENTER, OswImage::Alignment::START);
        hal->gfx()->setTextSize(2);
        hal->gfx()->setTextCenterAligned();
        hal->gfx()->setTextCursor(DISP_W / 2, 100);
        hal->gfx()->print("Hardware Problems");
        hal->gfx()->setTextSize(1);
        hal->gfx()->setTextCenterAligned();
        hal->gfx()->setTextCursor(DISP_W / 2, 120);
        hal->gfx()->print("If you see this screen, it means\nthat we detected some hardware\nproblems. Please check by\naware of the following:");
    
        short y = 160;
        hal->gfx()->setTextCursor(DISP_W / 2, y);
        hal->gfx()->print("No battery level with active wifi");
#if OSW_DEVICE_ESP32_WIFI_LOWPWR == 1
        y += 10;
        hal->gfx()->setTextCursor(DISP_W / 2, y);
        hal->gfx()->print("Active wifi may cause CPU brown-outs");
#endif
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
        y += 10;
        hal->gfx()->setTextCursor(DISP_W / 2, y);
        hal->gfx()->print("GPS reception is... Terrible.");
#endif
        hal->gfx()->setTextCursor(DISP_W / 2, 205);
        hal->gfx()->print("Press any button to continue.");
    } else {
        // Okay, we are done! Restore the original root app.
        OswUI::getInstance()->setRootApplication(this->previousRootApp);
        nvs.putString("v", GIT_COMMIT_HASH);
    }

    // Auto-hide the tutorial after one minute - in case something gets stuck
    if(time(nullptr) - this->timeout > 60)
        OswUI::getInstance()->setRootApplication(this->previousRootApp);
}

void OswAppTutorial::onButton(int id, bool up, OswAppV2::ButtonStateNames state) {
    OswAppV2::onButton(id, up, state); // always make sure to call the base class method!
    if(!up) return;
    this->timeout = time(nullptr); // reset the timeout, as the user interacted with the device
    if(this->screen == 0) {
        this->screen = 1;
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
        this->needsRedraw = true;
        if(this->gotButtonShort and this->gotButtonLong and this->gotButtonVeryLong and this->gotButtonDouble) {
            this->screen = 2;
            // Disable double press detection again (speeds up short press detection again)
            for(int i = 0; i < NUM_BUTTONS; i++)
                this->knownButtonStates[i] = (OswAppV2::ButtonStateNames) (this->knownButtonStates[i] ^ OswAppV2::ButtonStateNames::DOUBLE_PRESS); // Using XOR, as we know it was enabled before
        }
    } else if(this->screen == 2) {
        this->screen = 3;
    } else if(this->screen == 3) {
        this->screen = 4;
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

bool OswAppTutorial::changeRootAppIfNecessary() {
    if(nvs.getString("v", "") != String(GIT_COMMIT_HASH)) {
        this->previousRootApp = OswUI::getInstance()->getRootApplication();
        OswUI::getInstance()->setRootApplication(this);
        return true;
    }
    return false;
}