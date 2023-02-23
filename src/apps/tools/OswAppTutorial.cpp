#include <gfx_util.h>
#include <OswAppV1.h>
#include <osw_hal.h>

#include "apps/tools/OswAppTutorial.h"

const char* OswAppTutorial::getAppId() {
    return "org.osw.tutorial";
}

const char* OswAppTutorial::getAppName() {
    return "OSW Tutorial";
}

// void OswAppTutorial::onStart() {
//     OswAppV2::onStart(); // always make sure to call the base class method!
// }

// void OswAppTutorial::onLoop() {
//     OswAppV2::onLoop(); // always make sure to call the base class method!
// }

// void OswAppTutorial::onDraw() {
//     OswAppV2::onDraw(); // always make sure to call the base class method!
// }

// void OswAppTutorial::onStop() {
//     OswAppV2::onStop(); // always make sure to call the base class method!
// }
