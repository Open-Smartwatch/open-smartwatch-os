#include <iostream>
#include <thread>

#include <SDL2/SDL.h>

#include "../include/Emulator.hpp"

int main(int argc, char** argv) {
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
        printf("error initializing SDL: %s\n", SDL_GetError());
    {
        std::unique_ptr<OswEmulator> oswEmu = std::make_unique<OswEmulator>();
        OswEmulator::instance = oswEmu.get();
        oswEmu->run();
        OswEmulator::instance = nullptr;
    }
    SDL_Quit();

    return EXIT_SUCCESS;
}