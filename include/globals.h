#pragma once

#include <Arduino.h> // For RTC_DATA_ATTR

/**
 * As a rule of thumb: Try to avoid global variables, as they are not reset (e.g. inside the emulator),
 * because they are not bound to a lifetime of an object. This also applies to applications and their states.
 * Furthermore, their names may collide and cause unreleated bugs!
 *
 * Yes, they are helpful during early development, but should not be used inside the final code!
 */

/**
 * These are the current states of the main.cpp watchfaces and app switchers.
 * They are commonly initialized in the setup() function and then used in the loop() function.
 *
 * These are global variables, as the setup() and loop() functions are not part of any class.
 */
RTC_DATA_ATTR extern uint16_t main_watchFaceIndex;  // Will only be initialized after deep sleep inside the setup() method!
extern uint16_t main_currentAppIndex;               // -> wakeup from deep sleep returns to watch face (and allows auto sleep)
extern uint16_t main_fitnessAppIndex;
extern uint16_t main_clockAppIndex;
extern uint16_t main_settingsAppIndex;