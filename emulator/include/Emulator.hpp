#pragma once

#include <SDL2/SDL.h>

void setup();
void loop();

class OswEmulator {
public:
    OswEmulator();
    ~OswEmulator();

    void run();
private:
    SDL_Window* mainWindow = nullptr; // Do not delete() this, this is done by SDL2
    SDL_Renderer* mainRenderer = nullptr;
    bool running = true;
};