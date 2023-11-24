#include <memory>
#include <thread>
#include <optional>
#include <functional>

#include "utest.h"
#include "../fixtures/CaptureSerialFixture.hpp"
#include "../fixtures/PreferencesFixture.hpp"
#include "../fixtures/EmulatorFixture.hpp"
#include "../fixtures/OswAppV2Fixture.hpp"

#include "apps/examples/OswAppExampleV2.h"
#include "apps/watchfaces/OswAppWatchfaceMonotimer.h"
#include "apps/watchfaces/OswAppWatchfaceBinary.h"

#include <osw_hal.h>
#include <osw_ui.h>

extern int emulatorMainArgc;
extern char** emulatorMainArgv;

/// switch though some apps and make sure it worked (without crashing)
UTEST(OswAppV2Fixture, app_switching) {
    EMULATOR_FIXTURE_HEADLESS_VARIABLE;

    CaptureSerialFixture capture;
    PreferencesFixture prefsFixture;
    EmulatorFixture runEmu(headless);

    OswAppV2* original = OswUI::getInstance()->getRootApplication();
    {
        OswAppExampleV2 app;
        OswAppV2Fixture appFixture(app);
        for(int i = 0; i < 10; ++i) {
            EXPECT_TRUE(OswUI::getInstance()->getRootApplication() == &appFixture.getApp()); // nobody should have changed the root application
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    ASSERT_TRUE(OswUI::getInstance()->getRootApplication() == original); // should have reverted changed the root application

    {
        OswAppWatchfaceMonotimer app;
        OswAppV2Fixture appFixture(app);
        for(int i = 0; i < 10; ++i) {
            EXPECT_TRUE(OswUI::getInstance()->getRootApplication() == &appFixture.getApp());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    ASSERT_TRUE(OswUI::getInstance()->getRootApplication() == original);

    {
        OswAppWatchfaceBinary app;
        OswAppV2Fixture appFixture(app);
        for(int i = 0; i < 10; ++i) {
            EXPECT_TRUE(OswUI::getInstance()->getRootApplication() == &appFixture.getApp());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    ASSERT_TRUE(OswUI::getInstance()->getRootApplication() == original);
}