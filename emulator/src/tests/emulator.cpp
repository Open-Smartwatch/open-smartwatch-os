#include <memory>
#include <thread>
#include <optional>
#include <functional>

#include "utest.h"
#include "fixtures/EmulatorFixture.hpp"

#include <osw_hal.h>
#include <osw_config.h>

extern int emulatorMainArgc;
extern char** emulatorMainArgv;

UTEST(emulator, run_headless) {
    EmulatorFixture runEmu(true);
    std::this_thread::sleep_for(std::chrono::seconds(10));
}

UTEST(emulator, run_headless_getting_hal) {
    EmulatorFixture runEmu(true);
    OswHal* hal = OswHal::getInstance();
    ASSERT_NE(hal, nullptr); // The HAL should be available
}

UTEST(emulator, run_headless_test_wakeupconfigs) {
    EmulatorFixture runEmu(true);
    OswHal::WakeUpConfig config = {};
    config.time = time(nullptr) + 2;
    // Add the config once
    size_t cId1 = OswHal::getInstance()->addWakeUpConfig(config);
    assert(cId1 != 0);
    OswHal::getInstance()->removeWakeUpConfig(cId1);
    // And again
    size_t cId2 = OswHal::getInstance()->addWakeUpConfig(config);
    assert(cId1 != cId2);

    // Put OS to sleep
    runEmu.oswEmu->requestSleep(OswEmulator::RequestSleepState::deep);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Give the emulator some time to process the request
    ASSERT_TRUE(runEmu.oswEmu->getCpuState() == OswEmulator::CPUState::deep);
    
    // Wait for the wakeup
    while(time(nullptr) <= config.time)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_TRUE(runEmu.oswEmu->getCpuState() == OswEmulator::CPUState::active); // If this is not true, the emulator did not wake up again
}

UTEST(emulator, run_normal) {
    for(int i = 0; i < ::emulatorMainArgc; ++i)
        if(strcmp(::emulatorMainArgv[i], "--headless") == 0)
            UTEST_SKIP("Not to be executed in headless mode.");
    EmulatorFixture runEmu(false);
    std::this_thread::sleep_for(std::chrono::seconds(10));
}