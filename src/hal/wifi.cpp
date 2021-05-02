#include <mini-wifi.h>

#include "osw_config.h"
#include "osw_hal.h"

String wifiSsid = OswConfig::getInstance()->getString(OSW_CONFIG_WIFI_SSID);
String wifiPass = OswConfig::getInstance()->getString(OSW_CONFIG_WIFI_PASS);
MiniWifi wifi(DEVICE_NAME, wifiSsid.c_str(), wifiPass.c_str());

MiniWifi* OswHal::getWiFi(void) { return &wifi; }