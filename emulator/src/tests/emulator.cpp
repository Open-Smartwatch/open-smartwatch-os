#include <memory>
#include <thread>
#include <optional>
#include <functional>

#include "utest.h"
#include "fixtures/EmulatorFixture.hpp"

#include <osw_hal.h>

extern int emulatorMainArgc;
extern char** emulatorMainArgv;

UTEST(emulator, run_headless) {
    EmulatorFixture runEmu(true);
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

    // TODO somehow put OS to sleep and check if it wakes up via the emulator - but how?
    //OswHal::getInstance()->lightSleep();
}

UTEST(emulator, run_normal) {
    for(int i = 0; i < ::emulatorMainArgc; ++i)
        if(strcmp(::emulatorMainArgv[i], "--headless") == 0)
            UTEST_SKIP("Not to be executed in headless mode.");
    EmulatorFixture runEmu(false);
}