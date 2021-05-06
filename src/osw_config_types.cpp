#include <nvs_flash.h>
#include <rom/rtc.h>

#include "gfx_util.h"
#include "osw_config_types.h"

//Add your keys to this namespace (do not forget to also declare them inside the header)
namespace OswConfigAllKeys {
OswConfigKeyString wifiSsid("a", "WiFi", "WLAN SSID", "Your wifi name", WIFI_SSID);
OswConfigKeyString wifiPass("b", "WiFi", "WLAN Password", nullptr, WIFI_PASS);
OswConfigKeyRGB appWTFprimaryColor("c", "App: Watchface", "Primary color", nullptr, rgb565(179, 107, 0));
}

//And also here, if you want to make it available in the configuration ui
const unsigned char oswConfigKeysCount = 3;
OswConfigKey* oswConfigKeys[] = {
    &OswConfigAllKeys::wifiSsid,
    &OswConfigAllKeys::wifiPass,
    &OswConfigAllKeys::appWTFprimaryColor
};