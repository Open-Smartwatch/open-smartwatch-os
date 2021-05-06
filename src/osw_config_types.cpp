#include <nvs_flash.h>
#include <rom/rtc.h>

#include "gfx_util.h"
#include "osw_config_types.h"

//Add your keys to this namespace (do not forget to also declare them inside the header)
namespace OswConfigAllKeys {
OswConfigKeyString wifiSsid("a", "WiFi", "WLAN SSID", WIFI_SSID);
OswConfigKeyString wifiPass("a", "WiFi", "WLAN Password", WIFI_PASS);
OswConfigKeyRGB appWTFprimaryColor("b", "App: Watchface", "Primary color", rgb565(179, 107, 0));
}

//And also here, if you want to make it available in the configuration ui
OswConfigKey* OswConfigKeys[] = {
    &OswConfigAllKeys::wifiSsid,
    &OswConfigAllKeys::wifiPass,
    &OswConfigAllKeys::appWTFprimaryColor
};