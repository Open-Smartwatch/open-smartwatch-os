#include <iostream>
#include <thread>
#include <memory>

#include <SDL2/SDL.h>

#include "cmdline.h"
#include "utest.h"
UTEST_STATE();

#include "../include/Emulator.hpp"
#include "tests/uiTests/UiTest_main.hpp"

// Global variables, to allow the unit tests to access them
int emulatorMainArgc;
char **emulatorMainArgv;

int main(int argc, char **argv)
{
    // Save the command line arguments
    ::emulatorMainArgc = argc;
    ::emulatorMainArgv = argv;

    // Parse command line arguments
    cmdline::parser a;
    const std::string argRunUnitTests = "unit_tests";
    const std::string argListUnitTests = "list_tests";
    const std::string argUiTests = "ui_tests";
    a.add(argRunUnitTests, '\0', "run the unit test framework");
    a.add(argListUnitTests, '\0', "list all tests, one per line");
    a.add(argUiTests, '\0', "run emulator with UI tests window");
    a.add("headless", '\0', "do not open a window; use software-rendering only"); // Warning: This parameter name is also used in the unit-tests!
    a.parse_check(argc, argv);

    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        printf("error initializing SDL: %s\n", SDL_GetError());

    // Run the unit tests or the emulator
    int returnval = EXIT_SUCCESS;
    if (a.exist(argRunUnitTests))
    {
        // In this mode we won't enter the emulator itself, but instead just run the unit tests
        returnval = utest_main(argc, argv);
    }
    else if (a.exist(argListUnitTests))
    {
        // Change --list_tests to --list-tests, because utest expects --list-tests
        for (int i = 0; i < strlen(argv[1]); i++)
        {
            if (argv[1][i] == '_')
            {
                argv[1][i] = '-';
                break;
            }
        }
        returnval = utest_main(argc, argv);
    }
    else if (a.exist(argUiTests))
    {
        // Run the emulator together with the testing engine
        returnval = runUiTests();
    }
    else
    {
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