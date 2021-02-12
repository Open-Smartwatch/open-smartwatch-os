#include <mini-wifi.h>

#include "config.h"
#include "osw_hal.h"

MiniWifi wifi(DEVICE_NAME, WIFI_SSID, WIFI_PASS);

MiniWifi* OswHal::getWiFi(void) { return &wifi; }