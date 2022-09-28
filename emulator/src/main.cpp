#include <iostream>
#include <thread>

#include <SDL2/SDL.h>

#include "cmdline.h"
#include "utest.h"
UTEST_STATE();

#include "../include/Emulator.hpp"

int main(int argc, char** argv) {
    // Parse command line arguments
    cmdline::parser a;
    a.add("unit_tests", '\0', "run the unit test framework");
    a.parse_check(argc, argv);

    if (a.exist("unit_tests"))
        // In this mode we won't enter the emulator itself, but instead just run the unit tests
        return utest_main(argc, argv);

    // Initialize SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
        printf("error initializing SDL: %s\n", SDL_GetError());

    // Create and run the emulator
    {
        std::unique_ptr<OswEmulator> oswEmu = std::make_unique<OswEmulator>();
        OswEmulator::instance = oswEmu.get();
        oswEmu->run();
        OswEmulator::instance = nullptr;
    }
    SDL_Quit();

    return EXIT_SUCCESS;
}