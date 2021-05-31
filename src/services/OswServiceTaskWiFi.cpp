#include <time.h>
#include "./services/OswServiceTaskWiFi.h"

#include "osw_hal.h"
#include "services/OswServiceManager.h"

#if defined(ESP8266)
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <HTTPClient.h>
#include <WiFi.h>
#else
#error only esp8266 and esp32 are supported
#endif

void OswServiceTaskWiFi::setup(OswHal* hal) {
  OswServiceTask::setup(hal);
  this->enableWiFi();
}

/**
 * This provides the "Auto-AP"-Feature (create AP while wifi is unavailable)
 */
void OswServiceTaskWiFi::loop(OswHal* hal) {
  //TODO reconnect if client active and it should be connected -> m_enableClient
  if(this->m_enableClient) {
    if(this->m_autoAPTimeout and WiFi.status() == WL_CONNECTED) {
      //Nice - reset timeout
      this->m_autoAPTimeout = 0;

    } else if(!this->m_autoAPTimeout and WiFi.status() != WL_CONNECTED) {
      //Wifi is either disconnected or unavailable -> start timeout until we start our own ap
      this->m_autoAPTimeout = time(nullptr);
#ifdef DEBUG
      Serial.println("[AutoAP] Timeout activated: 10 seconds");
#endif
    }

    if(!this->m_enableStation and this->m_autoAPTimeout and this->m_autoAPTimeout < time(nullptr) - 10) { //10 seconds no network -> auto ap!
      this->enableStation();
      this->m_enabledStationByAutoAP = true;
#ifdef DEBUG
      Serial.println("[AutoAP] Active.");
#endif
    }
  }

  if(this->m_enabledStationByAutoAP and (WiFi.status() == WL_CONNECTED or !this->m_enableClient)) {
    this->disableStation();
    this->m_enabledStationByAutoAP = false;
#ifdef DEBUG
    Serial.println("[AutoAP] Inactive.");
#endif
  }

  //TODO autoreconnect
}

void OswServiceTaskWiFi::stop(OswHal* hal) {
  OswServiceTask::stop(hal);
  this->disableWiFi();
}

/**
 * Enables wifi with the configured properties (caches the SSID/Pwd NOW)
 */
void OswServiceTaskWiFi::enableWiFi() {
  this->m_enableWiFi = true;
  this->updateWiFiConfig();
  this->m_clientSSID = std::move(OswConfigAllKeys::wifiSsid.get());
  this->m_clientPass = std::move(OswConfigAllKeys::wifiPass.get());
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
 * Is either the station active or are we currently connected to an ssid?
 */
bool OswServiceTaskWiFi::isConnected() {
  return this->m_enableStation or WiFi.status() == WL_CONNECTED;
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

bool OswServiceTaskWiFi::isWiFiEnabled() {
  return this->m_enableClient;
}

/**
 * Connect to the wifi, using the provided credentials from the config...
 */
void OswServiceTaskWiFi::connectWiFi() {
  this->m_enableClient = true;
  this->updateWiFiConfig();
  WiFi.begin(this->m_clientSSID.c_str(), this->m_clientPass.c_str());
  this->m_autoAPTimeout = 0;
#ifdef DEBUG
  Serial.println("Connecting to SSID " + OswConfigAllKeys::wifiSsid.get() + "...");
#endif
}

void OswServiceTaskWiFi::disconnectWiFi() {
  this->m_enableClient = false;
  WiFi.disconnect(false);
  this->updateWiFiConfig();
#ifdef DEBUG
  Serial.println("Disconnected wifi client...");
#endif
}

IPAddress OswServiceTaskWiFi::getClientIP() {
  return this->m_enableClient ? WiFi.localIP() : IPAddress();
}

bool OswServiceTaskWiFi::isStationEnabled() {
  return this->m_enableStation;
}

void OswServiceTaskWiFi::enableStation() {
  this->m_enableStation = true;
  this->m_enabledStationByAutoAP = false; //Revoke AutoAP station control
  this->updateWiFiConfig(); //This enables ap support
  WiFi.softAP(this->m_stationSSID.c_str(), this->m_stationPass.c_str());
#ifdef DEBUG
  Serial.println("Enabled own station with SSID " + this->getStationSSID() + "...");
#endif
}

void OswServiceTaskWiFi::disableStation() {
  this->m_enableStation = false;
  WiFi.softAPdisconnect(false);
  this->updateWiFiConfig();
#ifdef DEBUG
  Serial.println("Disabled station mode...");
#endif
}

IPAddress OswServiceTaskWiFi::getStationIP() {
  return this->m_enableStation ? WiFi.softAPIP() : IPAddress();
}

const String& OswServiceTaskWiFi::getStationSSID() const {
  return this->m_stationSSID;
}

const String& OswServiceTaskWiFi::getStationPassword() const {
  return this->m_stationPass;
}

/**
 * This updates the wifi modem state; including mode & hostname
 */
void OswServiceTaskWiFi::updateWiFiConfig() {
#if defined(ESP8266)
  WiFi.hostname("NOT IMPLEMENTED");
#elif defined(ESP32)
  WiFi.setHostname("NOT IMPLEMENTED");
#endif

  if(this->m_enableWiFi and this->m_enableClient and this->m_enableStation) {
    WiFi.mode(WIFI_MODE_APSTA);
#ifdef DEBUG
      Serial.println(String(__FILE__) + ": Station & client");
#endif
  } else if(this->m_enableWiFi and this->m_enableClient) {
    WiFi.mode(WIFI_MODE_STA);
#ifdef DEBUG
      Serial.println(String(__FILE__) + ": Client");
#endif
  } else if(this->m_enableWiFi and this->m_enableStation) {
    WiFi.mode(WIFI_MODE_AP);
#ifdef DEBUG
      Serial.println(String(__FILE__) + ": Station");
#endif
  } else {
    WiFi.mode(WIFI_MODE_NULL);
#ifdef DEBUG
      Serial.println(String(__FILE__) + ": Off");
#endif
  }
}