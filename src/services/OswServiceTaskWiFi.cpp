#ifdef OSW_FEATURE_WIFI
#include <time.h>
#include "./services/OswServiceTaskWiFi.h"

#include "osw_hal.h"
#include "services/OswServiceManager.h"
#include <ESPmDNS.h>

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
        if(this->m_connectTimeout and WiFi.status() == WL_CONNECTED) {
            // Nice - reset timeout
            this->m_connectTimeout = 0;
            this->m_connectFailureCount = 0;
        } else if(!this->m_connectTimeout and WiFi.status() != WL_CONNECTED) {
            // Wifi is either disconnected or unavailable...
            // -> start timeout
            this->m_connectTimeout = time(nullptr);
            OSW_LOG_D("[Connection] Timeout activated: 10 seconds");
        }

        // Handling in case of 10 seconds without a successful connect
        if(this->m_connectTimeout and this->m_connectTimeout < time(nullptr) - 10) {
            ++this->m_connectFailureCount;
            OSW_LOG_D("[Connection] Timeout expired. Looking for alternatives (", this->m_connectFailureCount, ")...");
            this->m_connectTimeout = 0; // Disable connection timeout, we will re-enable it with the next iteration, if we still want to connect (and are not in AutoAP mode)

            // We have four options if our connection times out; the fourth is the auto-ap, which is may not available
            if(this->m_connectFailureCount % 4 == 3) {
                if(OswConfigAllKeys::wifiAutoAP.get()) {
                    if(!this->m_enableStation) {
                        if (OswConfigAllKeys::hostPasswordEnabled.get()) {
                            this->enableStation(OswConfigAllKeys::hostPass.get().c_str());
                        } else {
                            this->enableStation();
                        }
                        this->m_enabledStationByAutoAP = time(nullptr);
                        OSW_LOG_D("[AutoAP] Active for ", this->m_enabledStationByAutoAPTimeout, " seconds (password is ", this->m_stationPass.isEmpty() ? "-" : this->m_stationPass, ").");
                    }
                } else {
                    OSW_LOG_D("[AutoAP] Is disabled; no action taken.");
                }
            } else
                this->connectWiFi();
        }

        if(this->m_queuedNTPUpdate and WiFi.status() == WL_CONNECTED) {
            OswHal::getInstance()->devices()->esp32->triggerNTPUpdate();
            this->m_queuedNTPUpdate = false;
        }

        if (OswHal::getInstance()->devices()->esp32->checkNTPUpdate())
            OswHal::getInstance()->setUTCTime(OswHal::getInstance()->devices()->esp32->getUTCTime()); // And apply the ESP32's time to the watches primary time provider (whatever this may be)
    }

    // Disable the auto-ap in case we connected successfully, disabled client or after this->m_enabledStationByAutoAPTimeout seconds
    const bool autoAPTimedOut = (time(nullptr) - this->m_enabledStationByAutoAP) >= this->m_enabledStationByAutoAPTimeout;
    if(this->m_enabledStationByAutoAP and (WiFi.status() == WL_CONNECTED or !this->m_enableClient or autoAPTimedOut)) {
        String dbgInactRsn;
        if(WiFi.status() == WL_CONNECTED)
            dbgInactRsn = "WiFi connected.";
        else if(WiFi.status() == WL_CONNECTED)
            dbgInactRsn = "WiFi disabled.";
        else if(autoAPTimedOut)
            dbgInactRsn = "Expired.";
        OSW_LOG_D("[AutoAP] Inactive: ", dbgInactRsn);

        this->disableStation();
        if(autoAPTimedOut)
            this->connectWiFi();
        this->m_enabledStationByAutoAP = 0;
    }

    if(this->isConnected() and !this->m_enabledMDNS) {
        this->m_enabledMDNS = true;
        if(MDNS.begin(this->m_hostname.c_str())) {
            // Announce our HTTP service (always. As we have no way of not-publishing this service ↓)
            MDNS.addService("http", "tcp", 80);
            MDNS.enableWorkstation();
            OSW_LOG_D("[mDNS] Active.");
        } else
            OSW_LOG_E("[mDNS] Failed to start!");
    } else if(!this->isConnected() and this->m_enabledMDNS) {
        //Assuming this also removes any added service (as the lib does not expose any removeService() function)
        MDNS.disableWorkstation();
        MDNS.end();
        this->m_enabledMDNS = false;
        OSW_LOG_D("[mDNS] Inactive.");
    }
}

void OswServiceTaskWiFi::selectCredentials() {
    if(this->m_connectFailureCount % 4 == 0 and !OswConfigAllKeys::wifiSsid.get().isEmpty()) {
        this->m_clientSSID = OswConfigAllKeys::wifiSsid.get();
        this->m_clientPass = OswConfigAllKeys::wifiPass.get();
    } else if(this->m_connectFailureCount % 4 == 1 and !OswConfigAllKeys::fallbackWifiSsid1st.get().isEmpty()) {
        this->m_clientSSID = OswConfigAllKeys::fallbackWifiSsid1st.get();
        this->m_clientPass = OswConfigAllKeys::fallbackWifiPass1st.get();
    } else if(this->m_connectFailureCount % 4 == 2 and !OswConfigAllKeys::fallbackWifiSsid2nd.get().isEmpty()) {
        this->m_clientSSID = OswConfigAllKeys::fallbackWifiSsid2nd.get();
        this->m_clientPass = OswConfigAllKeys::fallbackWifiPass2nd.get();
    } else if(this->m_connectFailureCount % 4 == 3) {
        // Huh? AutoAP should be active now. If not, we can reuse the previous credentials.
    }
    // Note, in case an SSID is empty we will just retry the previous one...
}

void OswServiceTaskWiFi::stop() {
    this->disableWiFi();
    OswServiceTask::stop();
}

/**
 * Enables wifi with the configured properties (caches the SSID/Pwd NOW)
 */
void OswServiceTaskWiFi::enableWiFi() {
    this->m_enableWiFi = true;
    this->m_connectFailureCount = 0;
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
    this->m_hostname = OswConfigAllKeys::hostname.get();
    this->m_connectTimeout = 0; //First reset to avoid racing conditions
    this->m_enableClient = true;
    this->updateWiFiConfig();
    this->selectCredentials();
    if (this->m_clientPass.isEmpty()) {  // If public Wi-Fi without a password
        WiFi.begin(this->m_clientSSID.c_str());
    } else {
        WiFi.begin(this->m_clientSSID.c_str(), this->m_clientPass.c_str());
    }
    if(!this->m_queuedNTPUpdate)
        this->m_queuedNTPUpdate = OswConfigAllKeys::wifiAlwaysNTPEnabled.get();
    OSW_LOG_D("[Client] Connecting to SSID \"", this->m_clientSSID, "\"...");
}

void OswServiceTaskWiFi::disconnectWiFi() {
    this->m_enableClient = false;
    WiFi.disconnect(false);
    this->updateWiFiConfig();
    OSW_LOG_D("[Client] Disconnected.");
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
 * @param password Set the wifi password to this (at least 8 chars!), otherwise a random password will be chosen. This parameter can be ignored if the station password is inactive in the config.
 */
void OswServiceTaskWiFi::enableStation(const String& password) {
    const bool usePassword = OswConfigAllKeys::hostPasswordEnabled.get();
    this->m_hostname = OswConfigAllKeys::hostname.get();
    if(usePassword) {
        if(password.isEmpty() or password.length() < 8)
            //Generate password
            this->m_stationPass = String(random(10000000, 99999999)); //Generate random 8 chars long numeric password
        else
            this->m_stationPass = password;
    } else
        this->m_stationPass.clear(); // Clear the stored password, as we have it disabled anyways...
    this->m_enableStation = true;
    this->m_enabledStationByAutoAP = 0; //Revoke AutoAP station control
    this->updateWiFiConfig(); //This enables ap support
    if(usePassword)
        WiFi.softAP(this->m_hostname.c_str(), this->m_stationPass.c_str());
    else
        WiFi.softAP(this->m_hostname.c_str());
    OSW_LOG_D("[Station] Enabled own station with SSID ", this->getStationSSID(), "...");
}

void OswServiceTaskWiFi::disableStation() {
    this->m_enableStation = false;
    WiFi.softAPdisconnect(false);
    this->updateWiFiConfig();
    OSW_LOG_D("[Station] Disabled.");
}

void OswServiceTaskWiFi::toggleAPPassword() {
    OswConfig::getInstance()->enableWrite();
    OswConfigAllKeys::hostPasswordEnabled.set(!OswConfigAllKeys::hostPasswordEnabled.get());
    OSW_LOG_D("[AP Password] Enabled? ", OswConfigAllKeys::hostPasswordEnabled.get());
    OswConfig::getInstance()->disableWrite();
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

#if OSW_DEVICE_ESP32_WIFI_LOWPWR == 1
    if(this->m_enableWiFi and !this->m_lowPowerMode) {
        OSW_LOG_D("[Mode] This platform has the low power wifi mode enabled, probably due to hardware limitations. This will seriously impact your cpu performance and connection quality, but should prevent your device from crashing.");
        this->m_lowPwrPrevFreq = OswHal::getInstance()->getCPUClock();
        this->m_lowPwrPrevWifiPwr = WiFi.getTxPower();
        WiFi.setTxPower(WIFI_POWER_MINUS_1dBm); // https://github.com/Open-Smartwatch/open-smartwatch-os/issues/264#issue-1301361379
        OswHal::getInstance()->setCPUClock(80); // https://github.com/Open-Smartwatch/open-smartwatch-os/issues/264#issuecomment-1181386357
        this->m_lowPowerMode = true;
    } else if(!this->m_enableWiFi and this->m_lowPowerMode) {
        OSW_LOG_D("[Mode] Reverting low power wifi mode...");
        WiFi.setTxPower(this->m_lowPwrPrevWifiPwr);
        OswHal::getInstance()->setCPUClock(this->m_lowPwrPrevFreq);
        this->m_lowPowerMode = false;
    }
#endif

    // How about "WiFi.mode(WIFI_MODE_APSTA)"? Well, so far every ESP was unable to work with this properly. So we removed it.
    if(this->m_enableWiFi and this->m_enableStation) {
        WiFi.mode(WIFI_MODE_AP);
        OSW_LOG_D("[Mode] Station");
    } else if(this->m_enableWiFi and this->m_enableClient) {
        WiFi.mode(WIFI_MODE_STA);
        OSW_LOG_D("[Mode] Client");
    } else {
        WiFi.mode(WIFI_MODE_NULL);
        OSW_LOG_D("[Mode] Off");
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
