#include <iostream>
#include <thread>
#include <memory>

#include <SDL2/SDL.h>

#include "cmdline.h"
#include "utest.h"
UTEST_STATE();

#include "../include/Emulator.hpp"

// Global variables, to allow the unit tests to access them
int emulatorMainArgc;
char** emulatorMainArgv;

int main(int argc, char** argv) {
    // Save the command line arguments
    ::emulatorMainArgc = argc;
    ::emulatorMainArgv = argv;

    // Parse command line arguments
    cmdline::parser a;
    a.add("unit_tests", '\0', "run the unit test framework");
    a.add("headless", '\0', "do not open a window; use software-rendering only"); // Warning: This parameter name is also used in the unit-tests!
    a.parse_check(argc, argv);

    // Initialize SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
        printf("error initializing SDL: %s\n", SDL_GetError());

    // Run the unit tests or the emulator
    int returnval = EXIT_SUCCESS;
    if (a.exist("unit_tests")) {
        // In this mode we won't enter the emulator itself, but instead just run the unit tests
        returnval = utest_main(argc, argv);
    } else {
        // Create and run the emulator
        std::unique_ptr<OswEmulator> oswEmu = std::make_unique<OswEmulator>(a.exist("headless"));
        OswEmulator::instance = oswEmu.get();
        oswEmu->run();
        OswEmulator::instance = nullptr;
    }

    // Shutdown SDL
    SDL_Quit();

    return returnval;
}