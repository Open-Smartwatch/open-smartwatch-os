#include <mini-wifi.h>
#include <osw_config_keys.h>

#include "osw_hal.h"

String wifiSsid = OswConfigAllKeys::wifiSsid.get();
String wifiPass = OswConfigAllKeys::wifiPass.get();
MiniWifi wifi(DEVICE_NAME, wifiSsid.c_str(), wifiPass.c_str());

MiniWifi* OswHal::getWiFi(void) { return &wifi; }