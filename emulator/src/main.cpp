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
    const std::string argRunTests = "unit_tests";
    const std::string argListTests = "list-tests"; 
    a.add(argRunTests, '\0', "run the unit test framework");
    a.add(argListTests, '\0', "list all tests, one per line");
    a.parse_check(argc, argv);

    if (a.exist(argRunTests) || a.exist(argListTests))
        // In this mode we won't enter the emulator itself, but instead just run or list the unit tests
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