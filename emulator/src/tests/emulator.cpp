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
    EXPECT_NE(hal, nullptr); // The HAL should be available
}

static bool run_headless_test_wakeupconfigs_selected = false;
static bool run_headless_test_wakeupconfigs_used = false;
static bool run_headless_test_wakeupconfigs_expired = false;

UTEST(emulator, run_headless_test_wakeupconfigs) {
    ASSERT_FALSE(run_headless_test_wakeupconfigs_selected);
    ASSERT_FALSE(run_headless_test_wakeupconfigs_used);
    ASSERT_FALSE(run_headless_test_wakeupconfigs_expired);

    CaptureSerialFixture capture;
    EmulatorFixture runEmu(true);
    runEmu.oswEmu->autoWakeUp = false; // Disable the auto wakeup, so we can test it manually

    OswHal::WakeUpConfig config = {};
    config.time = time(nullptr) + 4;

    // Set callbacks - this is ONLY for testing here!
    // The pointers are not valid after this function returns!
    // This is not acceptable in real code, where the pointers have to persist across reboots!
    config.expired = []() {
        run_headless_test_wakeupconfigs_expired = true;
    };
    config.selected = []() {
        run_headless_test_wakeupconfigs_selected = true;
    };
    config.used = []() {
        run_headless_test_wakeupconfigs_used = true;
    };

    // Add the config once
    size_t cId1 = OswHal::getInstance()->addWakeUpConfig(config);
    assert(cId1 != 0);
    EXPECT_FALSE(run_headless_test_wakeupconfigs_selected);
    EXPECT_FALSE(run_headless_test_wakeupconfigs_used);
    EXPECT_FALSE(run_headless_test_wakeupconfigs_expired);
    OswHal::getInstance()->removeWakeUpConfig(cId1);
    EXPECT_FALSE(run_headless_test_wakeupconfigs_selected);
    EXPECT_FALSE(run_headless_test_wakeupconfigs_used);
    EXPECT_FALSE(run_headless_test_wakeupconfigs_expired);

    // And readd it
    size_t cId2 = OswHal::getInstance()->addWakeUpConfig(config);
    assert(cId1 != cId2);
    EXPECT_FALSE(run_headless_test_wakeupconfigs_selected);
    EXPECT_FALSE(run_headless_test_wakeupconfigs_used);
    EXPECT_FALSE(run_headless_test_wakeupconfigs_expired);

    // Put OS to sleep
    runEmu.oswEmu->requestSleep(OswEmulator::RequestSleepState::deep);
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Give the emulator some time to process the request
    EXPECT_TRUE(runEmu.oswEmu->getCpuState() == OswEmulator::CPUState::deep);
    EXPECT_TRUE(run_headless_test_wakeupconfigs_selected);
    EXPECT_FALSE(run_headless_test_wakeupconfigs_used);
    EXPECT_FALSE(run_headless_test_wakeupconfigs_expired);
    
    // Wait for the wakeup
    while(time(nullptr) <= config.time)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Longer it should not take for the cpu to wake up - not including OS bootup time
    EXPECT_TRUE(runEmu.oswEmu->getCpuState() == OswEmulator::CPUState::active); // If this is not true, the emulator did not wake up again
    EXPECT_TRUE(run_headless_test_wakeupconfigs_selected);
    EXPECT_TRUE(run_headless_test_wakeupconfigs_used);
    EXPECT_FALSE(run_headless_test_wakeupconfigs_expired);
}

UTEST(emulator, run_normal) {
    for(int i = 0; i < ::emulatorMainArgc; ++i)
        if(strcmp(::emulatorMainArgv[i], "--headless") == 0)
            UTEST_SKIP("Not to be executed in headless mode.");
    CaptureSerialFixture capture;
    EmulatorFixture runEmu(false);
    std::this_thread::sleep_for(std::chrono::seconds(10));
}