#include "osw_config_keys.h"

#include <nvs_flash.h>
#include <rom/rtc.h>

#include "gfx_util.h"

// Add your keys to this namespace (do not forget to also declare them inside the header)
namespace OswConfigAllKeys {
// TODO Translate all this?
OswConfigKeyString wifiSsid("a", "WiFi", "SSID", "Your wifi name", CONFIG_WIFI_SSID);
OswConfigKeyPassword wifiPass("b", "WiFi", "Password", nullptr, CONFIG_WIFI_PASS);
OswConfigKeyRGB themePrimaryColor("c", "Theme & UI", "Primary color", nullptr, rgb888(179, 107, 0));
OswConfigKeyShort displayTimeout("d", "Theme & UI", "Screen timeout", "Seconds until the screen blanks", 15);
OswConfigKeyString dateFormat("e", "Date & Time", "Date format", "Currently only \"mm/dd/yyyy\" and \"dd.mm.yyyy\" are supported. ", CONFIG_DATE_FORMAT);
OswConfigKeyFloat daylightOffset("f", "Date & Time", "Daylight offset",
                                 "Daylight saving time offset in hours (e.g. 0.5 = 30 min)", CONFIG_DAYLIGHTOFFSET);
OswConfigKeyBool timeFormat("g", "Date & Time", "Use 24h time format?", nullptr, true);
OswConfigKeyShort timeZone("h", "Date & Time", "Timezone", "Number of offset hours (e.g. 2 = Berlin).",
                           CONFIG_TIMEZONE);
}  // namespace OswConfigAllKeys

// And also here, if you want to make it available in the configuration ui
const unsigned char oswConfigKeysCount = 8;
OswConfigKey* oswConfigKeys[] = {&OswConfigAllKeys::wifiSsid,          &OswConfigAllKeys::wifiPass,
                                 &OswConfigAllKeys::themePrimaryColor, &OswConfigAllKeys::displayTimeout,
                                 &OswConfigAllKeys::dateFormat,        &OswConfigAllKeys::daylightOffset,
                                 &OswConfigAllKeys::timeZone,          &OswConfigAllKeys::timeFormat};