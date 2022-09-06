#pragma once

#include <array>
#include <atomic>
#include <SDL2/SDL.h>
#include <vector>
#include <array>
#include <variant>

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

    std::vector<std::variant<bool, float, int, std::string, std::array<float, 3>, short>> keyList;

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