#include <iostream>

#include <SDL2/SDL.h>

#include "../include/Emulator.hpp"

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        printf("error initializing SDL: %s\n", SDL_GetError());

    OswEmulator oswemu;
    oswemu.run();

    SDL_Quit();
    return EXIT_SUCCESS;
}