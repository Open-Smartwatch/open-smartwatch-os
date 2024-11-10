#include "globals.h"

#include "apps/OswAppDrawer.h"
namespace OswGlobals {
RTC_DATA_ATTR size_t main_AppIndex = OswAppDrawer::UNDEFINED_SLEEP_APP_INDEX;
OswAppDrawer main_mainDrawer{&main_AppIndex};
std::unique_ptr<OswAppTutorial> main_tutorialApp;
bool main_delayedAppInit = true;

#ifdef OSW_EMULATOR
void resetGlobals() {
    main_delayedAppInit = true;
    main_mainDrawer.reset();
}
#endif
}
