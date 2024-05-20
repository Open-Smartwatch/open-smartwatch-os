#pragma once

/**
 * This file contains ALL the possible language keys. You can add your own language by "#define"-ing your translated keys and then include this file at the end.
 * This is also the reason, why all the "#define"-s here are encapsulated in "#ifndef"-s: This way, you can define your own keys and they will be used instead of the English ones.
 * To simplify the maintenance of the language files, try to keep the keys in the same order as in this file.
 *
 * Happy translating!
 */


// General terms
#ifndef LANG_CONNECT
#define LANG_CONNECT "Connect"
#endif
#ifndef LANG_CONNECTED
#define LANG_CONNECTED "Connected"
#endif
#ifndef LANG_DISCONNECT
#define LANG_DISCONNECT "Disconnect"
#endif
#ifndef LANG_MANUALLY
#define LANG_MANUALLY "Manually"
#endif
#ifndef LANG_CANCEL
#define LANG_CANCEL "Cancel"
#endif
#ifndef LANG_SAVE
#define LANG_SAVE "Save"
#endif
#ifndef LANG_ALARM
#define LANG_ALARM "Alarm"
#endif
#ifndef LANG_NEXT
#define LANG_NEXT "Next"
#endif
#ifndef LANG_DAILY
#define LANG_DAILY "Daily"
#endif
#ifndef LANG_WEEKDAYS
#define LANG_WEEKDAYS "Weekdays"
#endif
#ifndef LANG_WEEKENDS
#define LANG_WEEKENDS "Weekends"
#endif
#ifndef LANG_TIMER
#define LANG_TIMER "Timer"
#endif
#ifndef LANG_START
#define LANG_START "Start"
#endif
#ifndef LANG_CATEGORY
#define LANG_CATEGORY "Category"
#endif
#ifndef LANG_APP
#define LANG_APP "App"
#endif
#ifndef LANG_OK
#define LANG_OK "OK"
#endif
#ifndef LANG_RED
#define LANG_RED "red"
#endif
#ifndef LANG_NORMAL
#define LANG_NORMAL "normal"
#endif
#ifndef LANG_IMAGE
#define LANG_IMAGE "Image"
#endif
#ifndef LANG_EXAMPLE
#define LANG_EXAMPLE "Example"
#endif
#ifndef LANG_EXAMPLES
#define LANG_EXAMPLES "Examples"
#endif
#ifndef LANG_ANALOG
#define LANG_ANALOG "Analog"
#endif
#ifndef LANG_DIGITAL
#define LANG_DIGITAL "Digital"
#endif
#ifndef LANG_WATCHFACES
#define LANG_WATCHFACES "Watchfaces"
#endif
#ifndef LANG_FITNESS
#define LANG_FITNESS "Fitness"
#endif
#ifndef LANG_TOOLS
#define LANG_TOOLS "Tools"
#endif
#ifndef LANG_WEATHER
#define LANG_WEATHER "Weather"
#endif
#ifndef LANG_SETTINGS
#define LANG_SETTINGS "Settings"
#endif
#ifndef LANG_GAMES
#define LANG_GAMES "Games"
#endif
#ifndef LANG_MIX
#define LANG_MIX "Mix"
#endif
#ifndef LANG_DUAL
#define LANG_DUAL "Dual"
#endif
#ifndef LANG_BINARY
#define LANG_BINARY "Binary"
#endif
#ifndef LANG_MONO
#define LANG_MONO "Mono"
#endif
#ifndef LANG_NUMERALS
#define LANG_NUMERALS "Numerals"
#endif
#ifndef LANG_AFIT
#define LANG_AFIT "Analog+Fit"
#endif

// App: Time from web
#ifndef LANG_TFW_UPDATE
#define LANG_TFW_UPDATE "Update"
#endif

// App: BLE media control
#ifndef LANG_BMC_VOLUME
#define LANG_BMC_VOLUME "Volume"
#endif
#ifndef LANG_BMC_CONNECTING
#define LANG_BMC_CONNECTING "Connecting..."
#endif

// App: Webserver
#ifndef LANG_WEBSRV_STATION_PWD
#define LANG_WEBSRV_STATION_PWD "Station Password:"
#endif
#ifndef LANG_WEBSRV_TITLE
#define LANG_WEBSRV_TITLE "Configuration"
#endif
#ifndef LANG_WEBSRV_USER
#define LANG_WEBSRV_USER "User:"
#endif
#ifndef LANG_WEBSRV_PASS
#define LANG_WEBSRV_PASS "Password:"
#endif
#ifndef LANG_WEBSRV_AP_PASSWORD_ON
#define LANG_WEBSRV_AP_PASSWORD_ON "AutoAP PW ON"
#endif
#ifndef LANG_WEBSRV_AP_PASSWORD_OFF
#define LANG_WEBSRV_AP_PASSWORD_OFF "AutoAP PW OFF"
#endif

// App: Watchface Binary
#ifndef LANG_WATCHFACE_BINARY_STEPS
#define LANG_WATCHFACE_BINARY_STEPS "steps"
#endif

// App: Watchface Fitness
#ifndef LANG_WATCHFACE_FITNESS_DISTANCE
#define LANG_WATCHFACE_FITNESS_DISTANCE "Dist"
#endif
#ifndef LANG_WATCHFACE_FITNESS_STEP
#define LANG_WATCHFACE_FITNESS_STEP "Step"
#endif

// App: Fitness Statistics
#ifndef LANG_FITNESS_TITLE
#define LANG_FITNESS_TITLE "Fitness stats"
#endif
#ifndef LANG_FITNESS_STEP
#define LANG_FITNESS_STEP "Step"
#endif
#ifndef LANG_FITNESS_DISTANCE
#define LANG_FITNESS_DISTANCE "Distance"
#endif
#ifndef LANG_FITNESS_TOTAL
#define LANG_FITNESS_TOTAL "Total"
#endif

// App: Step Statistics
#ifndef LANG_STEPSTATS_TITLE
#define LANG_STEPSTATS_TITLE "Steps stats"
#endif

// App: Kcal Statistics
#ifndef LANG_KCALSTATS_TITLE
#define LANG_KCALSTATS_TITLE "Kcal stats"
#endif

// App: Distance Statistics
#ifndef LANG_DISTSTATS_TITLE
#define LANG_DISTSTATS_TITLE "Dist stats"
#endif

// Apps: Examples
#ifndef LANG_EXAMPLES_HELLO_WORLD
#define LANG_EXAMPLES_HELLO_WORLD "Hello World"
#endif

// App: Tutorial
#ifndef LANG_TUT_ANYKEY
#define LANG_TUT_ANYKEY "Press any button to continue."
#endif
#ifndef LANG_TUT_SCR0_TITLE
#define LANG_TUT_SCR0_TITLE "Welcome!"
#endif
#ifndef LANG_TUT_SCR0_TEXT
#define LANG_TUT_SCR0_TEXT "This is your own Open-Smartwatch!\nIn the next few seconds this\n\"tutorial\" will guide you through\nthe basic navigation concepts\nof this OS. Have fun!"
#endif
#ifndef LANG_TUT_SCR1_TITLE
#define LANG_TUT_SCR1_TITLE "Navigation"
#endif
#ifndef LANG_TUT_SCR1_TEXT
#define LANG_TUT_SCR1_TEXT "Please press the button modes\nlisted below to continue.\nYou can open the app-drawer by\n long-pressing the SELECT-button."
#endif
#ifndef LANG_TUT_SCR1_SHORT_PRESS
#define LANG_TUT_SCR1_SHORT_PRESS "Short Press"
#endif
#ifndef LANG_TUT_SCR1_DOUBLE_PRESS
#define LANG_TUT_SCR1_DOUBLE_PRESS "Double Press"
#endif
#ifndef LANG_TUT_SCR1_LONG_PRESS
#define LANG_TUT_SCR1_LONG_PRESS "Long Press"
#endif
#ifndef LANG_TUT_SCR1_VERY_LONG_PRESS
#define LANG_TUT_SCR1_VERY_LONG_PRESS "Very long Press"
#endif
#ifndef LANG_TUT_SCR2_TITLE
#define LANG_TUT_SCR2_TITLE "Battery\nCalibration"
#endif
#ifndef LANG_TUT_SCR2_TEXT
#define LANG_TUT_SCR2_TEXT "As this hardware has no BMS,\nthe OS has to learn the battery\ncapacity on-the-fly. Make sure to\nfully discharge the battery if\nyou see the battery icon being"
#endif
#ifndef LANG_TUT_SCR2_TEXT_COLORED
#define LANG_TUT_SCR2_TEXT_COLORED "filled with this \"info\" color."
#endif
#ifndef LANG_TUT_SCR3_TITLE
#define LANG_TUT_SCR3_TITLE "Hardware Problems"
#endif
#ifndef LANG_TUT_SCR3_TEXT
#define LANG_TUT_SCR3_TEXT "If you see this screen, it means\nthat we detected some hardware\nproblems. Please be aware\nof the following:"
#endif
#ifndef LANG_TUT_SCR3_NOBATWIFI
#define LANG_TUT_SCR3_NOBATWIFI "No battery level with active wifi"
#endif
#ifndef LANG_TUT_SCR3_WIFILOWPW
#define LANG_TUT_SCR3_WIFILOWPW "Active wifi may cause CPU brown-outs"
#endif
#ifndef LANG_TUT_SCR3_CRAPGPS
#define LANG_TUT_SCR3_CRAPGPS "GPS reception is... Terrible."
#endif

// Weekdays
#ifndef LANG_SUNDAY
#define LANG_SUNDAY "Sunday"
#endif
#ifndef LANG_MONDAY
#define LANG_MONDAY "Monday"
#endif
#ifndef LANG_TUESDAY
#define LANG_TUESDAY "Tuesday"
#endif
#ifndef LANG_WEDNESDAY
#define LANG_WEDNESDAY "Wednesday"
#endif
#ifndef LANG_THURSDAY
#define LANG_THURSDAY "Thursday"
#endif
#ifndef LANG_FRIDAY
#define LANG_FRIDAY "Friday"
#endif
#ifndef LANG_SATURDAY
#define LANG_SATURDAY "Saturday"
#endif

// Weekdays (short)
#ifndef LANG_SUN
#define LANG_SUN "Sun"
#endif
#ifndef LANG_MON
#define LANG_MON "Mon"
#endif
#ifndef LANG_TUE
#define LANG_TUE "Tue"
#endif
#ifndef LANG_WED
#define LANG_WED "Wed"
#endif
#ifndef LANG_THU
#define LANG_THU "Thu"
#endif
#ifndef LANG_FRI
#define LANG_FRI "Fri"
#endif
#ifndef LANG_SAT
#define LANG_SAT "Sat"
#endif

// Emulator
#ifndef LANG_IMGUI_DISPLAY
#define LANG_IMGUI_DISPLAY "Display"
#endif
#ifndef LANG_IMGUI_DISPLAY_NOPE
#define LANG_IMGUI_DISPLAY_NOPE "Display is not active."
#endif
#ifndef LANG_IMGUI_EMULATOR
#define LANG_IMGUI_EMULATOR "Emulator"
#endif
#ifndef LANG_IMGUI_BUTTONS
#define LANG_IMGUI_BUTTONS "Buttons"
#endif
#ifndef LANG_IMGUI_VIRTUAL_SENSORS
#define LANG_IMGUI_VIRTUAL_SENSORS "Virtual Sensors"
#endif
#ifndef LANG_IMGUI_VIRTUAL_SENSORS_NOPE
#define LANG_IMGUI_VIRTUAL_SENSORS_NOPE "The virtual sensors are only available, while the virtual device is active."
#endif
#ifndef LANG_IMGUI_CONFIGURATION
#define LANG_IMGUI_CONFIGURATION "Configuration"
#endif
#ifndef LANG_IMGUI_CONFIGURATION_NOPE
#define LANG_IMGUI_CONFIGURATION_NOPE "The configuration is not initialized yet."
#endif
#ifndef LANG_EMULATOR_CPU_ACTIVE
#define LANG_EMULATOR_CPU_ACTIVE "Active"
#endif
#ifndef LANG_EMULATOR_CPU_LIGHT_SLEEP
#define LANG_EMULATOR_CPU_LIGHT_SLEEP "Light Sleep"
#endif
#ifndef LANG_EMULATOR_CPU_DEEP_SLEEP
#define LANG_EMULATOR_CPU_DEEP_SLEEP "Deep Sleep"
#endif
#ifndef LANG_EMULATOR_WAKELOCK
#define LANG_EMULATOR_WAKELOCK "Keep-Awake"
#endif
#ifndef LANG_EMULATOR_WAKELOCK_HELP
#define LANG_EMULATOR_WAKELOCK_HELP "This will always wakeup the watch for the next frame."
#endif
#ifndef LANG_EMULATOR_FPSLIMIT
#define LANG_EMULATOR_FPSLIMIT "FPS Limiter"
#endif
#ifndef LANG_EMULATOR_FPSLIMIT_HELP
#define LANG_EMULATOR_FPSLIMIT_HELP "This will limit the FPS to the target FPS set for the platform."
#endif
#ifndef LANG_EMULATOR_BTN
#define LANG_EMULATOR_BTN "Button"
#endif
#ifndef LANG_EMULATOR_BTN_PWR_HELP
#define LANG_EMULATOR_BTN_PWR_HELP "This button will interrupt the power to the CPU and reset the OS (as from deep sleep)."
#endif
#ifndef LANG_EMULATOR_MBTN
#define LANG_EMULATOR_MBTN "Release after multi-press"
#endif
#ifndef LANG_EMULATOR_MBTN_HELP
#define LANG_EMULATOR_MBTN_HELP "Whenever you press-and-hold any butten(s) by activating their checkbox(es) and then click-and-release any button normally, all other held buttons will also be released."
#endif