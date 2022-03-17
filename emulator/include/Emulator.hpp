#pragma once

#include <array>
#include <atomic>
#include <SDL2/SDL.h>

void setup();
void loop();

class OswEmulator {
public:
    static OswEmulator* instance; // "Singleton"

    OswEmulator();
    ~OswEmulator();

    void run();
    void exit();

    void setButton(unsigned id, bool state);
    bool getButton(unsigned id);
private:
    SDL_Window* mainWindow = nullptr; // Do not delete() this, this is done by SDL2
    SDL_Renderer* mainRenderer = nullptr;
    std::atomic_bool running = true;
    std::array<std::atomic_bool, 3> buttons; // TODO This length sould come from the platform itself!
};