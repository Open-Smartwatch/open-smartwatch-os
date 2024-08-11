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
#include "apps/tests/OswAppTestProgressWidget.h"

#include <osw_hal.h>
#include <osw_ui.h>

extern int emulatorMainArgc;
extern char** emulatorMainArgv;

UTEST(widgets, OswAppTestProgressWidget) {
    EMULATOR_FIXTURE_HEADLESS_VARIABLE;

    CaptureSerialFixture capture;
    PreferencesFixture prefsFixture;
    EmulatorFixture runEmu(headless);

    OswAppTestProgressWidget app;
    OswAppV2Fixture appFixture(app);
    while(!app.next()) // controlled by the app
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
