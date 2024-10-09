#ifdef OSW_FEATURE_WIFI
#ifndef OSW_SERVICE_TASKWIFI_H
#define OSW_SERVICE_TASKWIFI_H

#include "osw_service.h"

#if defined(ESP8266)
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <HTTPClient.h>
#include <WiFi.h> // needed for wifi_power_t in this header
#else
#error only esp8266 and esp32 are supported
#endif

class WiFiClass;

class OswServiceTaskWiFi : public OswServiceTask {
  public:
    OswServiceTaskWiFi() {};
    virtual void setup() override;
    virtual void loop() override; /// Calls enableWiFi();
    virtual void stop() override; /// Calls disableWiFi();

    //General netowrking stuff
    void setHostname();
    String getHostname();

    //WiFi (general)
    void enableWiFi(); /// Enables the chips wifi and starts scanning for networks.
    void disableWiFi();
    WiFiClass* getNativeHandler();
    bool isConnected();
    bool isEnabled();
    IPAddress getIP(); /// Either get ip of this ap client it connected and enabled or station if enabled
    void queueTimeUpdateViaNTP();
    int32_t getSignalStrength();
    uint8_t getSignalQuality();

    //WiFi (client)
    bool isWiFiEnabled();
    void connectWiFi();
    void disconnectWiFi();
    IPAddress getClientIP();

    //WiFi (station)
    bool isStationEnabled();
    void enableStation(const String& password = String(""));
    void disableStation();
    void toggleAPPassword();
    IPAddress getStationIP();
    const String& getStationSSID() const;
    const String& getStationPassword() const;

    ~OswServiceTaskWiFi() {};

  private:
    bool m_bootDone = false; // This triggers the async setup inside the loop
    bool m_enableWiFi = false;
    bool m_enableClient = false;
    bool m_enableStation = false;
    bool m_enabledMDNS = false;
    time_t m_enabledStationByAutoAP = 0;
    const time_t m_enabledStationByAutoAPTimeout = 10 * 60; // Maximum allowed time for the auto ap to stay active - after that it ALWAYS WILL TRY to reconnect
    bool m_queuedNTPUpdate = false; //Will be set to true it this feature is active
    String m_hostname;
    String m_clientSSID;
    String m_clientPass;
    String m_stationPass;
    time_t m_connectTimeout = 0;
    uint8_t m_connectFailureCount = 0;
#if OSW_DEVICE_ESP32_WIFI_LOWPWR == 1
    bool m_lowPowerMode = false;
    uint8_t m_lowPwrPrevFreq;
    wifi_power_t m_lowPwrPrevWifiPwr;
#endif

    void updateWiFiConfig();
    void selectCredentials();
};

#endif
#endif
