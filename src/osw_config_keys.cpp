#include "osw_config_keys.h"

#include <nvs_flash.h>
#include <rom/rtc.h>

#include "gfx_util.h"
#include "osw_util.h"

// Add your keys to this namespace (do not forget to also declare them inside the header)
namespace OswConfigAllKeys {
// TODO Translate all this?
#ifdef OSW_FEATURE_WIFI
OswConfigKeyString hostname("i", "WiFi", "Hostname", "Used e.g. for the wifi station", DEVICE_NAME);
#ifdef OSW_FEATURE_WIFI_ONBOOT
OswConfigKeyBool wifiBootEnabled("j", "WiFi", "Enable on boot", "This will drain your battery faster!", WIFI_ON_BOOT);
#endif
OswConfigKeyBool wifiAlwaysNTPEnabled("k", "WiFi", "Always fetch time (when connected)", nullptr, NTP_ALWAYS_ON_WIFI);
OswConfigKeyBool wifiAutoAP("l", "WiFi", "Enable Auto AP",
                            "When the connection to the wifi fails, just create an own wifi station.", WIFI_AUTO_AP);
OswConfigKeyString wifiSsid("a", "WiFi", "SSID", "Your wifi name", CONFIG_WIFI_SSID);
OswConfigKeyPassword wifiPass("b", "WiFi", "Password", nullptr, CONFIG_WIFI_PASS);
#endif

OswConfigKeyShort settingDisplayBrightness("s1", "Energy Settings", "Display Brightness", "From 0 to 255",
                                           DISPLAY_BRIGHTNESS);
OswConfigKeyShort settingDisplayTimeout("s2", "Energy Settings", "Display Timeout",
                                        "Seconds until the screen blanks (0 = disable)", DISPLAY_TIMEOUT);
OswConfigKeyBool settingDisplayOverlays("s3", "Energy Settings", "Display Overlays", "Show overlays (at all)",
                                        DISPLAY_OVERLAYS);
OswConfigKeyBool settingDisplayOverlaysOnWatchScreen("s4", "Energy Settings", "Display Watchface Overlays",
                                                     "Show overlays on watchfaces", DISPLAY_OVERLAYS_ON_WF);
OswConfigKeyBool raiseToWakeEnabled("s5", "Energy Settings", "Raise/Tilt to Wake", "Enables Raise to Wake",
                                    WAKE_FROM_RAISE);
OswConfigKeyShort raiseToWakeSensitivity("s6", "Energy Settings", "Raise to Wake Sensitivity",
                                         "TBD - experiment (8bit, 1 LSB = 8mg)", WAKE_FROM_RAISE_SENSITIVITY);
OswConfigKeyBool lightSleepEnabled("s7", "Energy Settings", "Light Sleep", "Use light sleep instead of deep sleep.",
                                   DO_LIGHT_SLEEP);
OswConfigKeyBool tapToWakeEnabled("s8", "Energy Settings", "Tap to Wake",
                                  "Enables Tap to Wake (If you select none, button 1 will wake the watch)",
                                  WAKE_FROM_TAP);
OswConfigKeyBool buttonToWakeEnabled("m", "Energy Settings", "Button to Wake", "Enables Button to wake",
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

OswConfigKeyDropDown dateFormat("e", "Date & Time", "Date format", "mm/dd/yyyy,dd.mm.yyyy", CONFIG_DATE_FORMAT);
OswConfigKeyFloat daylightOffset("f", "Date & Time", "Daylight offset",
                                 "Daylight saving time offset in hours (e.g. 0.5 = 30 min)", CONFIG_DAYLIGHTOFFSET);
OswConfigKeyBool timeFormat("g", "Date & Time", "Use 24h time format?", nullptr, true);
OswConfigKeyShort timeZone("h", "Date & Time", "Timezone", "Number of offset hours (e.g. 2 = Berlin).",
                           CONFIG_TIMEZONE);
OswConfigKeyInt stepsPerDay("s", "Fitness", "Steps per day", "> 0!", STEPS_PER_DAY);
}  // namespace OswConfigAllKeys

// ...and also here, if you want to load them during boot and make them available in the configuration ui
OswConfigKey *oswConfigKeys[] = {
    #ifdef OSW_FEATURE_WIFI
    // wifi (2)
    &OswConfigAllKeys::hostname, &OswConfigAllKeys::wifiSsid, &OswConfigAllKeys::wifiPass,
    #ifdef OSW_FEATURE_WIFI_ONBOOT
    &OswConfigAllKeys::wifiBootEnabled, 
    #endif
    &OswConfigAllKeys::wifiAlwaysNTPEnabled, &OswConfigAllKeys::wifiAutoAP,
    #endif
    // display (8)
    &OswConfigAllKeys::settingDisplayTimeout, &OswConfigAllKeys::settingDisplayBrightness,
    &OswConfigAllKeys::settingDisplayOverlays, &OswConfigAllKeys::settingDisplayOverlaysOnWatchScreen,
    &OswConfigAllKeys::raiseToWakeEnabled, &OswConfigAllKeys::raiseToWakeSensitivity,
    &OswConfigAllKeys::tapToWakeEnabled, &OswConfigAllKeys::lightSleepEnabled,
    // date + time (4)
    &OswConfigAllKeys::dateFormat, &OswConfigAllKeys::daylightOffset, &OswConfigAllKeys::timeZone,
    &OswConfigAllKeys::timeFormat,
    // colors (9)
    &OswConfigAllKeys::themeBackgroundColor, &OswConfigAllKeys::themeBackgroundDimmedColor,
    &OswConfigAllKeys::themeForegroundColor, &OswConfigAllKeys::themeForegroundDimmedColor,
    &OswConfigAllKeys::themePrimaryColor, &OswConfigAllKeys::themeInfoColor, &OswConfigAllKeys::themeSuccessColor,
    &OswConfigAllKeys::themeWarningColor, &OswConfigAllKeys::themeDangerColor,
    // fitness (1)
    &OswConfigAllKeys::stepsPerDay};
const unsigned char oswConfigKeysCount = OswUtil::size(oswConfigKeys);
