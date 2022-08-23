#pragma once

#include <array>
#include <atomic>
#include <SDL2/SDL.h>

#include "gfx_2d.h"
#include "gfx_util.h"

#include "config_defaults.h"
#include "config.h"

void setup();
void loop();

class OswEmulator {
public:
    class EmulatorSleep {
        // This is a dummy class so the execution of the loop() function can be instantly aborted whenever the emulator enters "sleep" mode
    };

    static OswEmulator* instance; // "Singleton"

    OswEmulator();
    ~OswEmulator();

    void run();
    void drawWatchOverlay();
    void exit();

    void setButton(unsigned id, bool state);
    bool getButton(unsigned id);
    uint8_t getBatteryRaw();
    bool isCharging();

    void reboot();
    void enterSleep(bool toDeepSleep);
    bool fromDeepSleep();
private:
    enum class CPUState {
        active,
        lightSpleep,
        deepSleep
    };
    Graphics2D* graphics2d;

    SDL_Window* mainWindow = nullptr; // Do not delete() this, this is done by SDL2
    SDL_Renderer* mainRenderer = nullptr;
    std::atomic_bool running = true;
    std::array<std::atomic_bool, 3> buttons; // TODO This length sould come from the platform itself!
    uint8_t batRaw = 0;
    bool charging = true;
    CPUState cpustate = CPUState::deepSleep;
    bool autoWakeUp = true;
    bool wakeUpNow = false;

    // Web-interface

    // CheckBox section
    bool checkBoxTimeFormat = false;
    bool checkBoxSettingDisplayOverlays = false;
    bool checkBoxSettingDisplayOverlaysOnWatchScreen = false;
    bool checkBoxRaiseToWakeEnabled = false;
    bool checkBoxButtonToWakeEnabled = false;
    bool checkBoxTapToWakeEnabled = false;
    bool checkBoxLightSleepEnabled = false;
    bool checkBoxStepsHistoryClear = false;

    // Color section
    float colorThemeBackgroundColor[3] = {0/ 255.0F, 0/ 255.0F, 0/ 255.0F};
    float colorThemeBackgroundDimmedColor[3] = {64/ 255.0F, 64/ 255.0F, 64/ 255.0F};
    float colorThemeForegroundColor[3] = {255/ 255.0F, 255/ 255.0F, 255/ 255.0F};
    float colorThemeForegroundDimmedColor[3] = {122/ 255.0F, 122/ 255.0F, 122/ 255.0F};
    float colorThemePrimaryColor[3] = {0/ 255.0F, 209/ 255.0F, 178/ 255.0F};
    float colorThemeInfoColor[3] = {32/ 255.0F, 156/ 255.0F, 238/ 255.0F};
    float colorThemeSuccessColor[3] = {72/ 255.0F, 199/ 255.0F, 116/ 255.0F};
    float colorThemeWarningColor[3] = {255/ 255.0F, 221/ 255.0F, 87/ 255.0F};
    float colorThemeDangerColor[3] = {255 / 255.0F, 56 / 255.0F, 96 / 255.0F};

    // Dropdown section
     int comboDateFormat = 0;
    int comboSettingDisplayDefaultWatchface = 0;

    // input editor (int) section
    int inputintSettingDisplayBrightness = DISPLAY_BRIGHTNESS;
    int inputintSettingDisplayTimeout = DISPLAY_TIMEOUT;
    int inputintRaiseToWakeSensitivity = WAKE_FROM_RAISE_SENSITIVITY;
    int inputintTimeZone = CONFIG_TIMEZONE;
    int inputintStepsPerDay = STEPS_PER_DAY;

    // input editor (float) section
    float inputfloatDaylightOffset = CONFIG_DAYLIGHTOFFSET;

    void* keyList[25] = {
        &inputintSettingDisplayTimeout
        ,&inputintSettingDisplayBrightness
        ,&checkBoxSettingDisplayOverlays
        ,&checkBoxSettingDisplayOverlaysOnWatchScreen
        ,&comboSettingDisplayDefaultWatchface
        ,&checkBoxButtonToWakeEnabled
        ,&checkBoxRaiseToWakeEnabled
        ,&inputintRaiseToWakeSensitivity
        ,&checkBoxTapToWakeEnabled
        ,&checkBoxLightSleepEnabled
        ,&comboDateFormat // notyet
        ,&inputfloatDaylightOffset
        ,&inputintTimeZone
        ,&checkBoxTimeFormat
        ,&colorThemeBackgroundColor
        ,&colorThemeBackgroundDimmedColor
        ,&colorThemeForegroundColor
        ,&colorThemeForegroundDimmedColor
        ,&colorThemePrimaryColor
        ,&colorThemeInfoColor
        ,&colorThemeSuccessColor
        ,&colorThemeWarningColor
        ,&colorThemeDangerColor
        ,&inputintStepsPerDay
        ,&checkBoxStepsHistoryClear
    };

    const unsigned reduceFlickerFrames = 600;

    // ImGui and window style / sizes
    const float guiPadding = 10;
    const float guiWidth = 256;

    // Timings
    std::array<float, 128> timesLoop;
    std::array<float, 128> timesFrames;

    void renderGUIFrame();
    void addGUIHelp(const char* msg);
};