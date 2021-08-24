#ifdef OSW_FEATURE_WIFI
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
  /**
   * @brief Enables the chips wifi and starts scanning for networks.
   * 
   */
  void enableWiFi();

  void disableWiFi();
  WiFiClass* getNativeHandler();
  bool isConnected();
  IPAddress getIP(); /// Either get ip of this ap client it connected and enabled or station if enabled
  void queueTimeUpdateViaNTP();

  /**
 * @brief Get the strength signal of the wifi.
 * 
 * More information about RSSI : https://en.wikipedia.org/wiki/Received_signal_strength_indication
 * 
 * The quality of the signal could be estimated from the getSignalQuality function.
 * 
 * @return int32_t Signal strength in dBm. 
 */
  int32_t getSignalStrength();

  /**
 * @brief Calculate a signal quality index from 0 to 100 based on the
 * RSSI signal.
 * 
 *    * 0 mean no signal
 *    * 100 is an excellent signal
 * 
 * @return uint8_t Quality index of the Wifi signal from 0 to 100
 */
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

  bool m_enableWiFi = false;
  bool m_enableClient = false;
  bool m_enableStation = false;
  bool m_enabledMDNS = false;
  bool m_enabledStationByAutoAP = false;
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