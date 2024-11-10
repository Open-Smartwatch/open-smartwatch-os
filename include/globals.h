#pragma once

#include <Arduino.h> // For RTC_DATA_ATTR
#include <memory>

#include "apps/OswAppDrawer.h"
#include "apps/tools/OswAppTutorial.h"

namespace OswGlobals {
/**
 * As a rule of thumb: Try to avoid global variables, as they are not reset (e.g. inside the emulator),
 * because they are not bound to a lifetime of an object. This also applies to applications and their states.
 * Furthermore, their names may collide and cause unrelated bugs!
 *
 * Yes, they are helpful during early development, but should not be used inside the final code!
 */

/**
 * These are the current states of the main.cpp watchfaces/drawers.
 * They are commonly initialized in the setup() function and then used in the loop() function.
 *
 * These are global variables, as the setup() and loop() functions are not part of any class.
 */
RTC_DATA_ATTR extern size_t main_AppIndex;
extern OswAppDrawer main_mainDrawer;
extern std::unique_ptr<OswAppTutorial> main_tutorialApp;
extern bool main_delayedAppInit;

#ifdef OSW_EMULATOR
void resetGlobals();
#endif
}
