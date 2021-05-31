#ifndef OSW_SERVICE_TASKWIFI_H
#define OSW_SERVICE_TASKWIFI_H

#include "osw_service.h"

class WiFiClass;

class OswServiceTaskWiFi : public OswServiceTask {
 public:
  OswServiceTaskWiFi(){};
  void setup(OswHal* hal);
  void loop(OswHal* hal); /// Calls enableWiFi();
  void stop(OswHal* hal); /// Calls disableWiFi();

  //General netowrking stuff
  void setHostname();
  String getHostname();

  //WiFi (general)
  void enableWiFi(); /// Enables the chips wifi and starts scanning for networks.
  void disableWiFi();
  WiFiClass* getNativeHandler();
  bool isConnected();
  IPAddress getIP(); /// Either get ip of this ap client it connected and enabled or station if enabled
  //TODO general ssid&pass method (if pass is from client it will return a masked string instead)

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
  const bool onlyOneModeSimultaneously = true; //The ESP32 has some problems broadcasting its SSID while using client & station

  bool m_enableWiFi = false;
  bool m_enableClient = false;
  bool m_enableStation = false;
  bool m_enabledStationByAutoAP = false;
  time_t m_autoAPTimeout = 0;
  String m_hostname;
  String m_clientSSID;
  String m_clientPass;
  String m_stationPass;

  void updateWiFiConfig();
};

#endif