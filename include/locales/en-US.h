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

// App: Time from web
#ifndef LANG_TFW_UPDATE
#define LANG_TFW_UPDATE "Update"
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

// App: Alarm
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

// App: Timer
#ifndef LANG_TIMER
#define LANG_TIMER "Timer"
#endif
#ifndef LANG_START
#define LANG_START "Start"
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