#include <mini-wifi.h>

#include "osw_config.h"
#include "osw_hal.h"

MiniWifi wifi(DEVICE_NAME, OswConfig::getInstance()->get(OSW_CONFIG_WIFI_SSID), OswConfig::getInstance()->get(OSW_CONFIG_WIFI_PASS));

MiniWifi* OswHal::getWiFi(void) { return &wifi; }