#ifdef OSW_FEATURE_WIFI
#ifndef OSW_SERVICE_TASKWIFI_H
#define OSW_SERVICE_TASKWIFI_H

#include "osw_service.h"

class WiFiClass;

class OswServiceTaskWiFi : public OswServiceTask {
 public:
  OswServiceTaskWiFi(){};
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
  IPAddress getStationIP();
  const String& getStationSSID() const;
  const String& getStationPassword() const;
  
  ~OswServiceTaskWiFi(){};

 private:
  //The ESP32 has some problems broadcasting its SSID while using client & station
  #ifdef OSW_FEATURE_WIFI_APST
  const bool onlyOneModeSimultaneously = false;
  #else
  const bool onlyOneModeSimultaneously = true;
  #endif

  bool m_bootDone = false; // This triggers the async setup inside the loop
  bool m_enableWiFi = false;
  bool m_enableClient = false;
  bool m_enableStation = false;
  bool m_enabledMDNS = false;
  time_t m_enabledStationByAutoAP = 0;
  const time_t m_enabledStationByAutoAPTimeout = 10 * 60; // Maximum allowed time for the auto ap to stay active - after that it ALLWAYS WILL TRY to reconnect
  bool m_queuedNTPUpdate = false; //Will be set to true it this feature is active
  bool m_waitingForNTPUpdate = false;
  time_t m_autoAPTimeout = 0;
  String m_hostname;
  String m_clientSSID;
  String m_clientPass;
  String m_stationPass;

  void updateWiFiConfig();
};

#endif
#endif