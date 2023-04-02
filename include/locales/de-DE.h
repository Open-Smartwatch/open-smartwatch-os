#pragma once

// General terms
#define LANG_CONNECT "Verbinden"
#define LANG_CONNECTED "Verbunden"
#define LANG_DISCONNECT "Trennen"
#define LANG_MANUALLY "Manuell"
#define LANG_CANCEL "Abbrechen"
#define LANG_SAVE "Speichern"
#define LANG_ALARM "Alarm"
#define LANG_NEXT "Weiter"
#define LANG_DAILY "Täglich"
#define LANG_WEEKDAYS "Wochentags"
#define LANG_WEEKENDS "Wochends"
#define LANG_TIMER "Timer"
#define LANG_START "Starten"
#define LANG_CATEGORY "Kategorie"
#define LANG_APP "Anwendung"
#define LANG_OK "OK"
#define LANG_RED "rot"
#define LANG_NORMAL "normal"
#define LANG_IMAGE "Bild"
#define LANG_EXAMPLE "Beispiel"
#define LANG_EXAMPLES "Beispiele"

// App: Time from web
#define LANG_TFW_UPDATE "Abfragen"

// App: BLE media control
#define LANG_BMC_VOLUME "Lautst."
#define LANG_BMC_CONNECTING "Verbinden..."

// App: Webserver
#define LANG_WEBSRV_STATION_PWD "Stationspasswort:"
#define LANG_WEBSRV_TITLE "Einstellungen"
#define LANG_WEBSRV_USER "Nutzer:"
#define LANG_WEBSRV_PASS "Passwort:"
#define LANG_WEBSRV_AP_PASSWORD_ON "AutoAP PW an"
#define LANG_WEBSRV_AP_PASSWORD_OFF "AutoAP PW aus"

// App: Watchface Binary
#define LANG_WATCHFACE_BINARY_STEPS "Schritte"

// App: Watchface Fitness
#define LANG_WATCHFACE_FITNESS_DISTANCE "Distanz"
#define LANG_WATCHFACE_FITNESS_STEP "Schritte"

// App: Fitness Statistics
#define LANG_FITNESS_TITLE "Fitness-Statistik"
#define LANG_FITNESS_STEP "Schritte"
#define LANG_FITNESS_DISTANCE "Distanz"
#define LANG_FITNESS_TOTAL "Insgesamt"

// App: Step Statistics
#define LANG_STEPSTATS_TITLE "Schritt-Statistik"

// App: Kcal Statistics
#define LANG_KCALSTATS_TITLE "Kcal-Statistik"

// App: Distance Statistics
#define LANG_DISTSTATS_TITLE "Distanz-Statistik"

// Apps: Examples
#define LANG_EXAMPLES_HELLO_WORLD "Hallo Welt"

// Weekdays
#define LANG_MONDAY "Montag"
#define LANG_TUESDAY "Dienstag"
#define LANG_WEDNESDAY "Mittwoch"
#define LANG_THURSDAY "Donnerstag"
#define LANG_FRIDAY "Freitag"
#define LANG_SATURDAY "Samstag"
#define LANG_SUNDAY "Sonntag"

// Weekdays (short)
#define LANG_SUN "Son"
#define LANG_MON "Mon"
#define LANG_TUE "Die"
#define LANG_WED "Mit"
#define LANG_THU "Don"
#define LANG_FRI "Fre"
#define LANG_SAT "Sam"

// Emulator
#define LANG_IMGUI_DISPLAY "Bildschirm"
#define LANG_IMGUI_DISPLAY_NOPE "Bildschirm is nicht aktiv."
#define LANG_IMGUI_EMULATOR "Emulator"
#define LANG_IMGUI_BUTTONS "Tasten"
#define LANG_IMGUI_VIRTUAL_SENSORS "Virtuelle Sensoren"
#define LANG_IMGUI_VIRTUAL_SENSORS_NOPE "Die virtuellen Sensoren sind nur verfügbar, wenn das virtuelle Gerät aktiv ist."
#define LANG_IMGUI_CONFIGURATION "Konfiguration"
#define LANG_IMGUI_CONFIGURATION_NOPE "Die Konfiguration wurde (noch) nicht geladen."
#define LANG_EMULATOR_CPU_ACTIVE "Aktiv"
#define LANG_EMULATOR_CPU_LIGHT_SLEEP "Dösen"
#define LANG_EMULATOR_CPU_DEEP_SLEEP "Tiefschlaf"
#define LANG_EMULATOR_WAKELOCK "Koffein-Modus"
#define LANG_EMULATOR_WAKELOCK_HELP "Dies wird the Uhr vor jedem gezeichneten Bild (wieder) aufwecken."
#define LANG_EMULATOR_FPSLIMIT "FPS Begrenzer"
#define LANG_EMULATOR_FPSLIMIT_HELP "Dies wird the FPS auf den Wert für die gegebene Platform begrenzen."
#define LANG_EMULATOR_BTN "Taste"
#define LANG_EMULATOR_BTN_PWR_HELP "Dies wird die Stromversorgung zur CPU unterbrechen und das Betriebssystem zurücksetzen (ähnlich zum " LANG_EMULATOR_CPU_DEEP_SLEEP ")."
#define LANG_EMULATOR_MBTN "Loslassen nach Multi-Tasten-Klick"
#define LANG_EMULATOR_MBTN_HELP "Wenn du eine beliebige Taste gedrückt hältst und dann eine andere Taste drückst, werden alle anderen gedrückten Tasten losgelassen."

// At the very last: Include English as a fallback -> any keys not found in the current language, they will be defined in English
#include "en-US.h"