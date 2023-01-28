#include <memory>
#include <thread>
#include <optional>
#include <functional>

#include "utest.h"
#include "CaptureSerial.hpp"
#include "../../include/Emulator.hpp"

#include <osw_hal.h>

extern int emulatorMainArgc;
extern char** emulatorMainArgv;

enum emulatorRunResults: int { // using int, so utest can print it
    STARTING,
    RUNNING,
    STOPPED,
    FAILED
};

/**
 * @brief Run the emulator in a thread
 *
 * @param headless
 * @param callback - if provided, the emualtor will be shutdown INSTANTLY after the callback finished (also it will be started instantly after startup)!
 * @return emulatorRunResults
 */
static emulatorRunResults runEmulator(bool headless, std::optional<std::function<void(OswEmulator*)>> callback = std::nullopt) {
    CaptureSerial capture; // Shutup the serial output
    emulatorRunResults state = emulatorRunResults::STARTING;
    try {
        // Create and run the headless emulator
        std::unique_ptr<OswEmulator> oswEmu = std::make_unique<OswEmulator>(headless);
        OswEmulator::instance = oswEmu.get();
        std::thread t([&]() {
            try {
                state = emulatorRunResults::RUNNING;
                oswEmu->run();
                state = emulatorRunResults::STOPPED;
            } catch(...) {
                state = emulatorRunResults::FAILED;
            }
        });
        t.detach(); // If we time out or fail, we don't want to wait for the thread to finish
        // Wait for the thread to start
        while(state == emulatorRunResults::STARTING) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        if(state == emulatorRunResults::RUNNING) {
            if(callback.has_value()) {
                callback.value()(oswEmu.get());
            } else {
                // Run for 10 seconds
                std::this_thread::sleep_for(std::chrono::seconds(10));
            }
        }
        // Stop the emulator
        oswEmu->exit();
        // Wait for the thread to finish
        std::chrono::time_point stopWaitStarted = std::chrono::steady_clock::now();
        while(state == emulatorRunResults::RUNNING or std::chrono::steady_clock::now() - stopWaitStarted > std::chrono::seconds(10) ) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        OswEmulator::instance = nullptr;
    } catch(...) {
        // Okay, that's bad!
        return emulatorRunResults::FAILED;
    }
    return state;
}

UTEST(emulator, run_headless) {
    emulatorRunResults state = runEmulator(true);
    // Check if the run passed
    ASSERT_EQ(state, emulatorRunResults::STOPPED);
}

UTEST(emulator, run_headless_getting_hal) {
    emulatorRunResults state = runEmulator(true, [&](OswEmulator* oswEmu) {
        ASSERT_NE(oswEmu, nullptr); // The emulator should be running
        OswHal* hal = OswHal::getInstance();
        ASSERT_NE(hal, nullptr); // The HAL should be available
    });
    // Check if the run passed
    ASSERT_EQ(state, emulatorRunResults::STOPPED);
}

UTEST(emulator, run_normal) {
    for(int i = 0; i < ::emulatorMainArgc; ++i)
        if(strcmp(::emulatorMainArgv[i], "--headless") == 0)
            UTEST_SKIP("Not to be executed in headless mode.");
    emulatorRunResults state = runEmulator(false);
    // Check if the run passed
    ASSERT_EQ(state, emulatorRunResults::STOPPED);
}