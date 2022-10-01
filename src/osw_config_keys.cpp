#include "osw_config_keys.h"

#include <nvs_flash.h>
#ifndef OSW_EMULATOR
#include <rom/rtc.h>
#endif

#include "gfx_util.h"
#include "osw_util.h"

/**
 * !!!WARNING!!!
 *
 * Never select the same label for two different keys (inside their constructor)! This will cause a
 * "feature" inside the webinterface to be activated, which will lead to an empty data.json to be saved.
 *
 * There is no way to recover the users data after this. Be caureful!
 *
 */

// Add your keys to this namespace (do not forget to also declare them inside the header)
namespace OswConfigAllKeys {
// TODO Translate all this?
#ifdef OSW_FEATURE_WIFI
OswConfigKeyString hostname("i", "WiFi", "Hostname", "Used e.g. for the wifi station", DEVICE_NAME);
OswConfigKeyBool hostPasswordEnabled("i3", "WiFi", "Enable AutoAP Password", nullptr, true);
OswConfigKeyPassword hostPass("i2", "WiFi", "AutoAP Password", "Password to use for AutoAP (leave empty to use random)", "");

#ifdef OSW_FEATURE_WIFI_ONBOOT
OswConfigKeyBool wifiBootEnabled("j", "WiFi", "Enable on boot", "This will drain your battery faster!", WIFI_ON_BOOT);
#endif
OswConfigKeyBool wifiAlwaysNTPEnabled("k", "WiFi", "Always fetch time (when connected)", nullptr, NTP_ALWAYS_ON_WIFI);
OswConfigKeyBool wifiAutoAP("l", "WiFi", "Enable AutoAP",
                            "When the connection to the wifi fails, just create an own wifi station.", WIFI_AUTO_AP);
OswConfigKeyString wifiSsid("a", "WiFi", "1st SSID", "Your wifi name", CONFIG_WIFI_SSID);
OswConfigKeyPassword wifiPass("b", "WiFi", "1st Password", nullptr, CONFIG_WIFI_PASS);
OswConfigKeyString fallbackWifiSsid1st("a1", "WiFi", "2nd SSID", "Leave empty to disable", CONFIG_FALLBACK_1ST_WIFI_SSID);
OswConfigKeyPassword fallbackWifiPass1st("b1", "WiFi", "2nd Password", nullptr, CONFIG_FALLBACK_1ST_WIFI_PASS);
OswConfigKeyString fallbackWifiSsid2nd("a2", "WiFi", "3rd SSID", "Leave empty to disable", CONFIG_FALLBACK_2ND_WIFI_SSID);
OswConfigKeyPassword fallbackWifiPass2nd("b2", "WiFi", "3rd Password", nullptr, CONFIG_FALLBACK_2ND_WIFI_PASS);
#endif

OswConfigKeyShort settingDisplayBrightness("s1", "Display", "Display Brightness", "From 0 to 255",
        DISPLAY_BRIGHTNESS);
OswConfigKeyShort settingDisplayTimeout("s2", "Display", "Display Timeout",
                                        "Seconds until the screen blanks (0 = disable)", DISPLAY_TIMEOUT);
OswConfigKeyBool settingDisplayOverlays("s3", "Display", "Display Overlays", "Show overlays at all", DISPLAY_OVERLAYS);
OswConfigKeyBool settingDisplayOverlaysOnWatchScreen("s4", "Display", "Display Watchface Overlays", nullptr, DISPLAY_OVERLAYS_ON_WF);
OswConfigKeyDropDown settingDisplayDefaultWatchface("n", "Display",
        "Default Watchface ID (analog, digital, mix, Dual-time, Fitness-tracking, binary, monotimer, numerals)", "0,1,2,3,4,5,6,7", String(CONFIG_DEFAULT_WATCHFACE_INDEX));
OswConfigKeyBool settingDisplayDualHourTick("h2", "Display", "Display Dual-Time Hour Tick", "Show dual time hour tick", false);
#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
OswConfigKeyBool settingDisplayStepsGoal("g1", "Display", "Display Steps Goal", "Show goal steps", true);
#endif
OswConfigKeyShort appSwitcherLongPress("d4", "Display", "App Switcher Long Press", "in ms", APPSWITCHER_LONG_PRESS);
OswConfigKeyShort appSwitcherSleepPress("d5", "Display", "App Switcher Sleep Press", "in ms", APPSWITCHER_SLEEP_TIMEOUT);
OswConfigKeyBool raiseToWakeEnabled("s5", "Power", "Raise/Tilt to Wake", "Enables Raise to Wake",
                                    WAKE_FROM_RAISE);
OswConfigKeyShort raiseToWakeSensitivity("s6", "Power", "Raise to Wake Sensitivity",
        "TBD - experiment (8bit, 1 LSB = 8mg)", WAKE_FROM_RAISE_SENSITIVITY);
OswConfigKeyBool lightSleepEnabled("s7", "Power", "Light Sleep", "Use light sleep instead of deep sleep.",
                                   DO_LIGHT_SLEEP);
OswConfigKeyBool tapToWakeEnabled("s8", "Power", "Tap to Wake",
                                  "Enables Tap to Wake (If you select none, button 1 will wake the watch)",
                                  WAKE_FROM_TAP);
OswConfigKeyBool buttonToWakeEnabled("m", "Power", "Button to Wake", "This will always be used, in case no other trigger is enabled.",
                                     WAKE_FROM_BTN1);

OswConfigKeyRGB themeBackgroundColor("c1", "Theme & UI", "Background color", nullptr, THEME_BACKROUND_COLOR);
OswConfigKeyRGB themeBackgroundDimmedColor("c8", "Theme & UI", "Background color (dimmed)", nullptr,
        THEME_BACKROUND_DIMMED_COLOR);
OswConfigKeyRGB themeForegroundColor("c2", "Theme & UI", "Foreground color", nullptr, THEME_FOREGROUND_COLOR);
OswConfigKeyRGB themeForegroundDimmedColor("c9", "Theme & UI", "Foreground color (dimmed)", nullptr,
        THEME_FOREGROUND_DIMMED_COLOR);
OswConfigKeyRGB themePrimaryColor("c3", "Theme & UI", "Primary color", nullptr, THEME_PRIMARY_COLOR);
OswConfigKeyRGB themeInfoColor("c4", "Theme & UI", "Info color", nullptr, THEME_INFO_COLOR);
OswConfigKeyRGB themeSuccessColor("c5", "Theme & UI", "Success color", nullptr, THEME_SUCCESS_COLOR);
OswConfigKeyRGB themeWarningColor("c6", "Theme & UI", "Warning color", nullptr, THEME_WARNING_COLOR);
OswConfigKeyRGB themeDangerColor("c7", "Theme & UI", "Danger color", nullptr, THEME_DANGER_COLOR);

OswConfigKeyDropDown dateFormat("e", "Date & Time", "Date format", "mm/dd/yyyy,dd.mm.yyyy,yy.mm/dd", CONFIG_DATE_FORMAT);
OswConfigKeyFloat daylightOffset("f", "Date & Time", "Daylight offset",
                                 "Daylight saving time offset in hours (e.g. 0.5 = 30 min)", CONFIG_DAYLIGHTOFFSET);
OswConfigKeyBool timeFormat("g", "Date & Time", "Use 24h time format?", nullptr, true);
OswConfigKeyShort timeZone("h", "Date & Time", "Timezone", "Number of offset hours (e.g. 2 = Berlin).",
                           CONFIG_TIMEZONE);
OswConfigKeyShort dualTimeZone("h1", "Date & Time", "Dual-Timezone", "Number of offset Dual-hours (e.g. 9 = Seoul).", 0);
OswConfigKeyShort resetDay("r", "Date & Time", "Day of the week",
                           "Choose the day of the week to reset the number of steps (e.g. 1-7 are days, 0 is disabled).", 0);
#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
OswConfigKeyShort configHeight("f4", "Fitness", "User Height", "E.g 175.7 cm -> 175 (Rounds off)", 175);
OswConfigKeyShort configWeight("f5", "Fitness", "User Weight", "E.g 70.3 kg -> 70 (Rounds off)", 70);
OswConfigKeyInt stepsPerDay("f1", "Fitness", "Steps per day", "> 0!", STEPS_PER_DAY);
OswConfigKeyInt distPerDay("f2", "Fitness", "Distance per day", "> 0!", DIST_PER_DAY);
OswConfigKeyInt kcalPerDay("f3", "Fitness", "kcalorie per day", "> 0!", KCAL_PER_DAY);
OswConfigKeyBool stepsHistoryClear("o", "Fitness", "Clear historical days", "In case the watch did not run for multiple days, these will be cleared. Can make problems if time is lost during sleep.", STEPS_HISTORY_CLEAR);
#endif
}  // namespace OswConfigAllKeys

// ...and also here, if you want to load them during boot and make them available in the configuration ui
OswConfigKey* oswConfigKeys[] = {
#ifdef OSW_FEATURE_WIFI
    // wifi
    &OswConfigAllKeys::hostname,
    &OswConfigAllKeys::wifiSsid, &OswConfigAllKeys::wifiPass,
    &OswConfigAllKeys::fallbackWifiSsid1st,&OswConfigAllKeys::fallbackWifiPass1st,
    &OswConfigAllKeys::fallbackWifiSsid2nd,&OswConfigAllKeys::fallbackWifiPass2nd,
#ifdef OSW_FEATURE_WIFI_ONBOOT
    & OswConfigAllKeys::wifiBootEnabled,
#endif
    & OswConfigAllKeys::wifiAlwaysNTPEnabled, &OswConfigAllKeys::wifiAutoAP,
    &OswConfigAllKeys::hostPasswordEnabled, &OswConfigAllKeys::hostPass,
#endif
    // display
    &OswConfigAllKeys::settingDisplayTimeout, &OswConfigAllKeys::settingDisplayBrightness,
    &OswConfigAllKeys::settingDisplayOverlays, &OswConfigAllKeys::settingDisplayOverlaysOnWatchScreen,
    &OswConfigAllKeys::settingDisplayDefaultWatchface, &OswConfigAllKeys::settingDisplayDualHourTick,
    &OswConfigAllKeys::appSwitcherLongPress, &OswConfigAllKeys::appSwitcherSleepPress,
#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
    & OswConfigAllKeys::settingDisplayStepsGoal,
#endif
    // energy
    &OswConfigAllKeys::buttonToWakeEnabled, &OswConfigAllKeys::raiseToWakeEnabled,
    &OswConfigAllKeys::raiseToWakeSensitivity, &OswConfigAllKeys::tapToWakeEnabled,
    &OswConfigAllKeys::lightSleepEnabled,
    // date + time
    &OswConfigAllKeys::dateFormat, &OswConfigAllKeys::daylightOffset, &OswConfigAllKeys::timeZone,
    &OswConfigAllKeys::dualTimeZone, &OswConfigAllKeys::timeFormat, &OswConfigAllKeys::resetDay,
    // colors
    &OswConfigAllKeys::themeBackgroundColor, &OswConfigAllKeys::themeBackgroundDimmedColor,
    &OswConfigAllKeys::themeForegroundColor, &OswConfigAllKeys::themeForegroundDimmedColor,
    &OswConfigAllKeys::themePrimaryColor, &OswConfigAllKeys::themeInfoColor, &OswConfigAllKeys::themeSuccessColor,
    &OswConfigAllKeys::themeWarningColor, &OswConfigAllKeys::themeDangerColor,
#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1
    // fitness
    &OswConfigAllKeys::configHeight, &OswConfigAllKeys::configWeight, &OswConfigAllKeys::stepsPerDay,
    &OswConfigAllKeys::distPerDay, &OswConfigAllKeys::kcalPerDay, &OswConfigAllKeys::stepsHistoryClear
#endif
};
const unsigned char oswConfigKeysCount = OswUtil::size(oswConfigKeys);
