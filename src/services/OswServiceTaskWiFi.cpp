#ifdef OSW_FEATURE_WIFI
#include <time.h>
#include "./services/OswServiceTaskWiFi.h"

#include "osw_hal.h"
#include "services/OswServiceManager.h"
#include <ESPmDNS.h>

#if defined(ESP8266)
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <HTTPClient.h>
#include <WiFi.h>
#else
#error only esp8266 and esp32 are supported
#endif

void OswServiceTaskWiFi::setup() {
  OswServiceTask::setup();
  this->disableStation(); // Never enable station mode after boot
  this->m_bootDone = false;
}

/**
 * This provides the "Auto-AP"-Feature (create AP while wifi is unavailable)
 */
void OswServiceTaskWiFi::loop() {
  if(!this->m_bootDone) {
    #ifdef OSW_FEATURE_WIFI_ONBOOT
    if(OswConfigAllKeys::wifiBootEnabled.get()) {
      delay(2000); // Give the user some time to take a look at the battery level (as it is unavailable with enabled wifi)
      this->enableWiFi();
      this->connectWiFi();
    } else {
    #endif
    this->disableWiFi();
    #ifdef OSW_FEATURE_WIFI_ONBOOT
    }
    #endif
    this->m_bootDone = true;
  }

  if(this->m_enableClient) {
    if(this->m_autoAPTimeout and WiFi.status() == WL_CONNECTED) {
      //Nice - reset timeout
      this->m_autoAPTimeout = 0;
    } else if(!this->m_autoAPTimeout and WiFi.status() != WL_CONNECTED) {
      //Wifi is either disconnected or unavailable -> start timeout until we start our own ap
      this->m_autoAPTimeout = time(nullptr);
#ifndef NDEBUG
      Serial.println(String(__FILE__) + ": [AutoAP] Timeout activated: 10 seconds");
#endif
    }

    if(OswConfigAllKeys::wifiAutoAP.get() && !this->m_enableStation and this->m_autoAPTimeout and this->m_autoAPTimeout < time(nullptr) - 10) { //10 seconds no network -> auto ap!
      this->enableStation();
      this->m_enabledStationByAutoAP = time(nullptr);
#ifndef NDEBUG
      Serial.println(String(__FILE__) + ": [AutoAP] Active for " + String(this->m_enabledStationByAutoAPTimeout) + " seconds (password is " + this->m_stationPass + ").");
#endif
    }

    if(this->m_queuedNTPUpdate and WiFi.status() == WL_CONNECTED) {
      OswHal::getInstance()->devices->esp32->triggerNTPUpdate();
      this->m_queuedNTPUpdate = false;
      this->m_waitingForNTPUpdate = true;
#ifndef NDEBUG
      Serial.println(String(__FILE__) + ": [NTP] Started update...");
#endif
    }

    // sometimes time(nullptr) returns seconds since boot
    // so check the request was resolved
    if (this->m_waitingForNTPUpdate and time(nullptr) > 1600000000) {
      this->m_waitingForNTPUpdate = false;
#ifndef NDEBUG
      Serial.println(String(__FILE__) + ": [NTP] Update finished (time of " + time(nullptr) + ")!");
#endif
      OswHal::getInstance()->setUTCTime(time(nullptr));
    }
  }

  // Disable the auto-ap in case we connected successfully, disabled client or after this->m_enabledStationByAutoAPTimeout seconds
  const bool autoAPTimedOut = (time(nullptr) - this->m_enabledStationByAutoAP) >= this->m_enabledStationByAutoAPTimeout;
  if(this->m_enabledStationByAutoAP and (WiFi.status() == WL_CONNECTED or !this->m_enableClient or autoAPTimedOut)) {
    this->disableStation();
    if(autoAPTimedOut)
      this->connectWiFi();
    this->m_enabledStationByAutoAP = 0;
#ifndef NDEBUG
    Serial.println(String(__FILE__) + ": [AutoAP] Inactive.");
#endif
  }

  if(this->isConnected() and !this->m_enabledMDNS) {
    this->m_enabledMDNS = true;
    if(MDNS.begin(this->m_hostname.c_str())) {
      //Announce our HTTP service (always. as we have no way of not-publishing this service ↓)
      MDNS.addService("http", "tcp", 80);
      MDNS.enableWorkstation();
#ifndef NDEBUG
    Serial.println(String(__FILE__) + ": [mDNS] Active.");
#endif
    } else
      Serial.println(String(__FILE__) + ": mDNS start failed!");
  } else if(!this->isConnected() and this->m_enabledMDNS) {
    //Assuming this also removes any added service (as the lib does not expose any removeService() function) 
    MDNS.disableWorkstation();
    MDNS.end();
    this->m_enabledMDNS = false;
#ifndef NDEBUG
    Serial.println(String(__FILE__) + ": [mDNS] Inactive.");
#endif
  }
}

void OswServiceTaskWiFi::stop() {
  OswServiceTask::stop();
  this->disableWiFi();
}

/**
 * Enables wifi with the configured properties (caches the SSID/Pwd NOW)
 */
void OswServiceTaskWiFi::enableWiFi() {
  this->m_enableWiFi = true;
  this->updateWiFiConfig();
}

/**
 * Disables wifi, this will not modify isStationEnabled or isWiFiEnabled as these are properties
 */
void OswServiceTaskWiFi::disableWiFi() {
  this->m_enableWiFi = false;
  this->updateWiFiConfig();
}

WiFiClass* getNativeHandler() {
  return &WiFi;
}

/**
 * Is the wifi modem active in any way?
 */
bool OswServiceTaskWiFi::isEnabled() {
  return this->m_enableWiFi or this->m_enableStation;
}

/**
 * Is either the station active or are we currently connected to an ssid (and is the wifi modem enabled)?
 */
bool OswServiceTaskWiFi::isConnected() {
  return this->m_enableWiFi and (this->m_enableStation or WiFi.status() == WL_CONNECTED);
}

/**
 * Returns either the station ip or the client ip (if connected), otherwise reports IPAddress()
 */
IPAddress OswServiceTaskWiFi::getIP() {
  if(WiFi.status() == WL_CONNECTED)
    return WiFi.localIP();
  else if(this->m_enableStation)
    return WiFi.softAPIP();
  return IPAddress();
}

void OswServiceTaskWiFi::queueTimeUpdateViaNTP() {
  this->m_queuedNTPUpdate = true;
}

bool OswServiceTaskWiFi::isWiFiEnabled() {
  return this->m_enableClient;
}

/**
 * Connect to the wifi, using the provided credentials from the config...
 */
void OswServiceTaskWiFi::connectWiFi() {
  this->m_hostname = std::move(OswConfigAllKeys::hostname.get());
  this->m_autoAPTimeout = 0; //First reset to avoid racing conditions
  this->m_clientSSID = std::move(OswConfigAllKeys::wifiSsid.get());
  this->m_clientPass = std::move(OswConfigAllKeys::wifiPass.get());
  this->m_enableClient = true;
  this->updateWiFiConfig();
  WiFi.begin(this->m_clientSSID.c_str(), this->m_clientPass.c_str());
  if(!this->m_queuedNTPUpdate)
    this->m_queuedNTPUpdate = OswConfigAllKeys::wifiAlwaysNTPEnabled.get();
#ifndef NDEBUG
  Serial.println(String(__FILE__) + ": [Client] Connecting to SSID " + OswConfigAllKeys::wifiSsid.get() + "...");
#endif
}

void OswServiceTaskWiFi::disconnectWiFi() {
  this->m_enableClient = false;
  WiFi.disconnect(false);
  this->updateWiFiConfig();
#ifndef NDEBUG
  Serial.println(String(__FILE__) + ": [Client] Disconnected wifi client...");
#endif
}

IPAddress OswServiceTaskWiFi::getClientIP() {
  return this->m_enableClient ? WiFi.localIP() : IPAddress();
}

bool OswServiceTaskWiFi::isStationEnabled() {
  return this->m_enableStation;
}

/**
 * This enables the wifi station mode
 * 
 * @param password Set the wifi password to this (at least 8 chars!), otherwise a random password will be choosen.
 */
void OswServiceTaskWiFi::enableStation(const String& password) {
  this->m_hostname = std::move(OswConfigAllKeys::hostname.get());
  if(password.isEmpty())
    //Generate password
    this->m_stationPass = String(random(10000000, 99999999)); //Generate random 8 chars long numeric password
  else
    this->m_stationPass = password;
  this->m_enableStation = true;
  this->m_enabledStationByAutoAP = 0; //Revoke AutoAP station control
  this->updateWiFiConfig(); //This enables ap support
  WiFi.softAP(this->m_hostname.c_str(), this->m_stationPass.c_str());
#ifndef NDEBUG
  Serial.println(String(__FILE__) + ": [Station] Enabled own station with SSID " + this->getStationSSID() + "...");
#endif
}

void OswServiceTaskWiFi::disableStation() {
  this->m_enableStation = false;
  WiFi.softAPdisconnect(false);
  this->updateWiFiConfig();
#ifndef NDEBUG
  Serial.println(String(__FILE__) + ": [Station] Disabled station mode...");
#endif
}

IPAddress OswServiceTaskWiFi::getStationIP() {
  return this->m_enableStation ? WiFi.softAPIP() : IPAddress();
}

const String& OswServiceTaskWiFi::getStationSSID() const {
  return this->m_hostname;
}

const String& OswServiceTaskWiFi::getStationPassword() const {
  return this->m_stationPass;
}

/**
 * This updates the wifi modem state; including hostname, mdns & mode
 */
void OswServiceTaskWiFi::updateWiFiConfig() {
#if defined(ESP8266)
  WiFi.hostname(this->m_hostname.c_str());
#elif defined(ESP32)
  WiFi.setHostname(this->m_hostname.c_str());
#endif

  if(!this->onlyOneModeSimultaneously and this->m_enableWiFi and this->m_enableClient and this->m_enableStation) {
    WiFi.mode(WIFI_MODE_APSTA);
#ifndef NDEBUG
      Serial.println(String(__FILE__) + ": [Mode] Station & client");
#endif
  } else if(this->m_enableWiFi and this->m_enableStation) {
    //Check this BEFORE the client, so in case of onlyOneModeSimultaneously we prefer the station, when enabled!
    WiFi.mode(WIFI_MODE_AP);
#ifndef NDEBUG
      Serial.println(String(__FILE__) + ": [Mode] Station");
#endif
  } else if(this->m_enableWiFi and this->m_enableClient) {
    WiFi.mode(WIFI_MODE_STA);
#ifndef NDEBUG
      Serial.println(String(__FILE__) + ": [Mode] Client");
#endif
  } else {
    WiFi.mode(WIFI_MODE_NULL);
#ifndef NDEBUG
      Serial.println(String(__FILE__) + ": [Mode] Off");
#endif
  }
}

int32_t OswServiceTaskWiFi::getSignalStrength() {
  //Shamelessly copied from the MiniWiFi library
  return WiFi.RSSI();
}
uint8_t OswServiceTaskWiFi::getSignalQuality() {
  //Shamelessly copied from the MiniWiFi library
  int32_t rssi = getSignalStrength();
  uint8_t quality = 0;
  if (rssi < -100) {
    quality = 0;
  } else if (rssi > -50) {
    quality = 100;
  } else {
    quality = 2 * (rssi + 100);
  }
  return quality;
}
#endif