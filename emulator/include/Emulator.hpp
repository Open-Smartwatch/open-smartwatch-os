#pragma once

#include <array>
#include <atomic>
#include <SDL2/SDL.h>

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
    void exit();

    void setButton(unsigned id, bool state);
    bool getButton(unsigned id);
    uint8_t getBatteryRaw();
    bool isCharging();

    void enterSleep(bool toDeepSleep);
    bool fromDeepSleep();
private:

    SDL_Window* mainWindow = nullptr; // Do not delete() this, this is done by SDL2
    SDL_Renderer* mainRenderer = nullptr;
    std::atomic_bool running = true;
    std::array<std::atomic_bool, 3> buttons; // TODO This length sould come from the platform itself!
    uint8_t batRaw = 0;
    bool charging = true;
    bool deepSleeped = true;
};