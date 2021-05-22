#include "osw_config_keys.h"

#include <nvs_flash.h>
#include <rom/rtc.h>

#include "gfx_util.h"

// Add your keys to this namespace (do not forget to also declare them inside the header)
namespace OswConfigAllKeys {
// TODO Translate all this?
OswConfigKeyString wifiSsid("a", "WiFi", "SSID", "Your wifi name", CONFIG_WIFI_SSID);
OswConfigKeyPassword wifiPass("b", "WiFi", "Password", nullptr, CONFIG_WIFI_PASS);

OswConfigKeyShort settingDisplayBrightness("s1", "Settings", "Display Brightness", "from 0 to 255", 128);
OswConfigKeyShort settingDisplayTimeout("s2", "Settings", "Display Timeout", "Seconds until the screen blanks", 10);
OswConfigKeyBool settingDisplayOverlays("s3", "Settings", "Display Overlays", "Show overlays (at all)", true);
OswConfigKeyBool settingDisplayOverlaysOnWatchScreen("s4", "Settings", "Display Watchface Overlays",
                                                     "Show overlays on watchfaces", false);
OswConfigKeyBool settingRaiseToWakeEnabled("s5", "Settings", "Raise to Wake", "Enables Raise to Wake", false);
OswConfigKeyShort settingRaiseToWakeSensitivity("s6", "Settings", "Raise to Wake Sensitivity",
                                                "TBD - experiment (8bit, 1 LSB = 8mg)", 127);

OswConfigKeyRGB themeBackgroundColor("c1", "Theme & UI", "Background color", nullptr, rgb888(0, 0, 0));
OswConfigKeyRGB themeBackgroundDimmedColor("c8", "Theme & UI", "Background color (dimmed)", nullptr,
                                           rgb888(64, 64, 64));
OswConfigKeyRGB themeForegroundColor("c2", "Theme & UI", "Foreground color", nullptr, rgb888(255, 255, 255));
OswConfigKeyRGB themeForegroundDimmedColor("c9", "Theme & UI", "Foreground color (dimmed)", nullptr,
                                           rgb888(192, 192, 192));
OswConfigKeyRGB themePrimaryColor("c3", "Theme & UI", "Primary color", nullptr, rgb888(0, 209, 178));
OswConfigKeyRGB themeInfoColor("c4", "Theme & UI", "Info color", nullptr, rgb888(32, 156, 238));
OswConfigKeyRGB themeSuccessColor("c5", "Theme & UI", "Success color", nullptr, rgb888(72, 199, 116));
OswConfigKeyRGB themeWarningColor("c6", "Theme & UI", "Warning color", nullptr, rgb888(255, 221, 87));
OswConfigKeyRGB themeDangerColor("c7", "Theme & UI", "Danger color", nullptr, rgb888(255, 56, 96));

OswConfigKeyDropDown dateFormat("e", "Date & Time", "Date format", "mm/dd/yyyy,dd.mm.yyyy", CONFIG_DATE_FORMAT);
OswConfigKeyFloat daylightOffset("f", "Date & Time", "Daylight offset",
                                 "Daylight saving time offset in hours (e.g. 0.5 = 30 min)", CONFIG_DAYLIGHTOFFSET);
OswConfigKeyBool timeFormat("g", "Date & Time", "Use 24h time format?", nullptr, true);
OswConfigKeyShort timeZone("h", "Date & Time", "Timezone", "Number of offset hours (e.g. 2 = Berlin).",
                           CONFIG_TIMEZONE);
}  // namespace OswConfigAllKeys

// ...and also here, if you want to load them during boot and make them available in the configuration ui
const unsigned char oswConfigKeysCount = 21;  // <------------- DON'T FORGET THIS ONE IF YOU EDIT BELOW ;)
OswConfigKey* oswConfigKeys[] = {
    // wifi (2)
    &OswConfigAllKeys::wifiSsid, &OswConfigAllKeys::wifiPass,
    // colors (9)
    &OswConfigAllKeys::themeBackgroundColor, &OswConfigAllKeys::themeBackgroundDimmedColor,
    &OswConfigAllKeys::themeForegroundColor, &OswConfigAllKeys::themeForegroundDimmedColor,
    &OswConfigAllKeys::themePrimaryColor, &OswConfigAllKeys::themeInfoColor,     //
    &OswConfigAllKeys::themeSuccessColor, &OswConfigAllKeys::themeWarningColor,  //
    &OswConfigAllKeys::themeDangerColor,
    // display (6)
    &OswConfigAllKeys::settingDisplayTimeout, &OswConfigAllKeys::settingDisplayBrightness,
    &OswConfigAllKeys::settingDisplayOverlays, &OswConfigAllKeys::settingDisplayOverlaysOnWatchScreen,
    &OswConfigAllKeys::settingRaiseToWakeEnabled, &OswConfigAllKeys::settingRaiseToWakeSensitivity,
    // date + time (4)
    &OswConfigAllKeys::dateFormat, &OswConfigAllKeys::daylightOffset,  //
    &OswConfigAllKeys::timeZone, &OswConfigAllKeys::timeFormat,        //
};
