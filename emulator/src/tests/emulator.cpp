#include <memory>
#include <thread>
#include <optional>
#include <functional>

#include "utest.h"
#include "fixtures/CaptureSerialFixture.hpp"
#include "fixtures/EmulatorFixture.hpp"

#include <osw_hal.h>
#include <osw_config.h>

extern int emulatorMainArgc;
extern char** emulatorMainArgv;

UTEST(emulator, run_headless) {
    CaptureSerialFixture capture;
    EmulatorFixture runEmu(true);
    std::this_thread::sleep_for(std::chrono::seconds(10));
}

UTEST(emulator, run_headless_getting_hal) {
    CaptureSerialFixture capture;
    EmulatorFixture runEmu(true);
    OswHal* hal = OswHal::getInstance();
    ASSERT_NE(hal, nullptr); // The HAL should be available
}

UTEST(emulator, run_headless_test_wakeupconfigs) {
    CaptureSerialFixture capture;
    EmulatorFixture runEmu(true);
    runEmu.oswEmu->autoWakeUp = false; // Disable the auto wakeup, so we can test it manually

    OswHal::WakeUpConfig config = {};
    config.time = time(nullptr) + 4;

    // Add the config once
    size_t cId1 = OswHal::getInstance()->addWakeUpConfig(config);
    assert(cId1 != 0);
    OswHal::getInstance()->removeWakeUpConfig(cId1);
    // And again
    size_t cId2 = OswHal::getInstance()->addWakeUpConfig(config);
    assert(cId1 != cId2);

    // Put OS to sleep
    runEmu.oswEmu->requestSleep(OswEmulator::RequestSleepState::deep);
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Give the emulator some time to process the request
    ASSERT_TRUE(runEmu.oswEmu->getCpuState() == OswEmulator::CPUState::deep);
    
    // Wait for the wakeup
    while(time(nullptr) <= config.time)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Longer it should not take for the cpu to wake up - not including OS bootup time
    ASSERT_TRUE(runEmu.oswEmu->getCpuState() == OswEmulator::CPUState::active); // If this is not true, the emulator did not wake up again
}

UTEST(emulator, run_normal) {
    for(int i = 0; i < ::emulatorMainArgc; ++i)
        if(strcmp(::emulatorMainArgv[i], "--headless") == 0)
            UTEST_SKIP("Not to be executed in headless mode.");
    CaptureSerialFixture capture;
    EmulatorFixture runEmu(false);
    std::this_thread::sleep_for(std::chrono::seconds(10));
}