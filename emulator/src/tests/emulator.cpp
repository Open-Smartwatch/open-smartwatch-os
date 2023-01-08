#include <memory>
#include <thread>

#include "utest.h"
#include "../../include/Emulator.hpp"

extern int emulatorMainArgc;
extern char** emulatorMainArgv;

static int runEmulator(bool headless) {
    int state = 0; // 0 = starting, 1 = running, 2 = stopped, 3 = failed
    try {
        // Create and run the headless emulator
        std::unique_ptr<OswEmulator> oswEmu = std::make_unique<OswEmulator>(headless);
        OswEmulator::instance = oswEmu.get();
        std::thread t([&]() {
            try {
                state = 1;
                oswEmu->run();
                state = 2;
            } catch(...) {
                state = 3;
            }
        });
        t.detach(); // If we time out or fail, we don't want to wait for the thread to finish
        // Wait for the thread to start
        while(state == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        // Run for 10 seconds
        std::this_thread::sleep_for(std::chrono::seconds(10));
        // Stop the emulator
        oswEmu->exit();
        // Wait for the thread to finish
        std::chrono::time_point stopWaitStarted = std::chrono::steady_clock::now();
        while(state == 1 or std::chrono::steady_clock::now() - stopWaitStarted > std::chrono::seconds(10) ) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        OswEmulator::instance = nullptr;
    } catch(...) {
        // Okay, that's bad!
        return 2;
    }
    return state;
}

UTEST(emulator, run_headless) {
    int state = runEmulator(true);
    // Check if the run passed
    ASSERT_EQ(state, 2);
}

UTEST(emulator, run_normal) {
    for(int i = 0; i < ::emulatorMainArgc; ++i)
        if(strcmp(::emulatorMainArgv[i], "--headless") == 0)
            UTEST_SKIP("Not to be executed in headless mode.");
    int state = runEmulator(false);
    // Check if the run passed
    ASSERT_EQ(state, 2);
}