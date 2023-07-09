#pragma once

#include <array>
#include <atomic>
#include <SDL2/SDL.h>
#include <vector>
#include <array>
#include <variant>
#include <list>
#include <map>

#include "Jzon.h"

void setup();
void loop();

class OswEmulator {
  public:
    class EmulatorSleep {
        // This is a dummy class so the execution of the loop() function can be instantly aborted whenever the emulator enters "sleep" mode
    };
    enum class CPUState {
        active,
        light,
        deep
    };
    enum class RequestSleepState {
        nothing,
        light,
        deep
    };
    enum class BootReason {
        undefined,
        byUser,
        byAuto,
        byTimer
    };

    static OswEmulator* instance; // "Singleton"
    const bool isHeadless;
    bool autoWakeUp = true;

    OswEmulator(bool headless, std::string configPath = "config.json", std::string imguiPath = "imgui.ini");
    ~OswEmulator();

    void run();
    void exit();

    void setButton(unsigned id, bool state);
    bool getButton(unsigned id);
    uint8_t getBatteryRaw();
    bool isCharging();

    void cleanup();
    void reboot();
    bool fromDeepSleep();
    BootReason getBootReason();
    void requestSleep(RequestSleepState state);
    CPUState getCpuState();

    // Following functions are only used by the emulator / OS main loop itself
    void scheduleWakeupAfterSleep(unsigned long microseconds);
    void enterSleep(bool toDeepSleep);
  private:

    SDL_Window* mainWindow = nullptr; // Do not delete() this, this is done by SDL2
    SDL_Surface* mainSurface = nullptr; // Only used in headless mode
    SDL_Renderer* mainRenderer = nullptr;
    std::atomic_bool running = true;
    std::array<std::atomic_bool, 3> buttons; // TODO This length should come from the platform itself!
    std::array<bool, 3> buttonCheckboxes = { false, false, false }; // These are just state caches of the buttons for their respective checkboxes!
    bool buttonResetAfterMultiPress = true;
    uint8_t batRaw = 0;
    bool charging = true;
    CPUState cpustate = CPUState::deep;
    bool manualWakeUp = false;
    bool wantCleanup = false;
    std::vector<std::variant<bool, float, int, std::string, std::array<float, 3>, short>> configValuesCache;
    std::map<std::string, std::list<size_t>> configSectionsToIdCache;
    unsigned int lastUiFlush = 0;
    RequestSleepState requestedSleepState = RequestSleepState::nothing;
    unsigned long selfWakeUpInMicroseconds = 0;
    time_t selfWakeUpAtTimestamp = 0;
    BootReason bootReason = BootReason::undefined;

    // ImGui and window style / sizes
    const float guiPadding = 10;
    const float guiWidth = 256;

    // Timings
    std::array<float, 128> timesLoop{0.0f};
    std::array<float, 129> frameCountsEmulator{0.0f}; // One more frame count to not count current value
    std::array<float, 129> frameCountsOsw{0.0f};
    time_t frameCountsLastUpdate = 0;

    std::string configPath;
    std::string imguiPath;
    Jzon::Node config;

    void doCleanup();
    void renderGUIFrameEmulator();
    void addGUIHelp(const char* msg);

    // For UI Tests (to access and test private members)
    // Note: such friend classes are the only changes in production code related to testing
    friend class TestEmulator;
};