#include <esp_adc_cal.h>

#ifndef OSW_EMULATOR
#include "driver/rtc_io.h"
#endif
#include "osw_hal.h"
#include "osw_ui.h"
#include "osw_pins.h"
#include "OswAppV2.h"

#include <services/OswServiceTaskWiFi.h>
#include <services/OswServiceTasks.h>

#define V_REF 1100  // ADC reference voltage
#define CHANNEL ADC2_CHANNEL_8

// Some functions to determine the capacity of batteries
// see https://github.com/rlogiacco/BatterySense
// define one of the following
#define BATT_ASIGMOIDAL
//#define BATT_SIGMOIDAL
//#define BATT_LINEAR
//#define BATT_LEGACY

/**
 * Symmetric sigmoidal approximation
 * https://www.desmos.com/calculator/7m9lu26vpy
 *
 * c - c / (1 + k*x/v)^3
 */
static inline uint8_t sigmoidal(uint16_t voltage, uint16_t minVoltage, uint16_t maxVoltage) {
    int volt_diff = maxVoltage - minVoltage;
    if ( volt_diff <= 0)
        volt_diff = 1;

    // slow
    // uint8_t result = 110 - (110 / (1 + pow(1.468f * (voltage - minVoltage)/(volt_diff), 6f)));

    // steep
    // uint8_t result = 102 - (102 / (1 + pow(1.621f * (voltage - minVoltage)/(volt_diff), 8.1f)));

    // normal
    uint8_t result = 105 - (105 / (1 + powf(1.724f * (voltage - minVoltage) / volt_diff, 5.5f)));
    return result >= 100 ? 100 : result;
}

/**
 * Asymmetric sigmoidal approximation
 * https://www.desmos.com/calculator/oyhpsu8jnw
 *
 * c - c / [1 + (k*x/v)^4.5]^3
 */
static inline uint8_t asigmoidal(uint16_t voltage, uint16_t minVoltage, uint16_t maxVoltage) {
    int volt_diff = maxVoltage - minVoltage;
    if ( volt_diff <= 0)
        volt_diff = 1;
    uint8_t result = 101 - (101 / powf(1 + powf(1.33f * (voltage - minVoltage) / volt_diff, 4.5f), 3));
    return result >= 100 ? 100 : result;
}

/**
 * Linear mapping
 * https://www.desmos.com/calculator/sowyhttjta
 *
 * x * 100 / v
 */
static inline uint8_t linear(uint16_t voltage, uint16_t minVoltage, uint16_t maxVoltage) {
    int volt_diff = maxVoltage - minVoltage;
    if ( volt_diff <= 0)
        volt_diff = 1;

    return (unsigned long)(voltage - minVoltage) * 100 / volt_diff;
}

uint16_t OswHal::getBatteryRawMin() {
    return this->powerPreferences.getUShort("-", 60); // Every battery should be able to deliver lower than this at some point
}

uint16_t OswHal::getBatteryRawMax() {
    return this->powerPreferences.getUShort("+", 26); // Every battery should be able to deliver more than this
}

void OswHal::setupPower(bool fromLightSleep) {
    if(!fromLightSleep) {
#if OSW_DEVICE_TPS2115A_STATPWR != 0
        pinMode(OSW_DEVICE_TPS2115A_STATPWR, INPUT);
#endif
#if OSW_DEVICE_ESP32_BATLVL != 0
        pinMode(OSW_DEVICE_ESP32_BATLVL, INPUT);
#endif
        bool res = powerPreferences.begin("osw-power", false);
        assert(res && "Could not initialize power preferences!");
#if OSW_PLATFORM_DEFAULT_CPUFREQ != 0
        this->setCPUClock(OSW_PLATFORM_DEFAULT_CPUFREQ);
#else
#warning "OSW_PLATFORM_DEFAULT_CPUFREQ is not set, so CPU frequency won't be configured!"
#endif
    }
    esp_sleep_wakeup_cause_t reason = esp_sleep_get_wakeup_cause();
    if(reason == ESP_SLEEP_WAKEUP_TIMER) {
        OSW_LOG_D("Wakeup from timer - checking for remaining config...");
        // Determine if a wakeup config was used -> if so, call the callback
        std::optional<OswHal::WakeUpConfig> config = this->readAndResetWakeUpConfig(fromLightSleep);
        if(config.has_value()) {
            OSW_LOG_D("Wakeup config found!");
            if(config.value().used)
                config.value().used();
        }
    }
    this->noteUserInteraction();
}

void OswHal::stopPower() {
    powerPreferences.end();
}

/**
 * Update the power statistics, ignores unrealistic battery values (value must be 10 < v < 80) and only works during dischrging and without wifi enabled (bug on current hardware revisions)
 */
void OswHal::updatePowerStatistics(uint16_t currBattery) {
    this->expireWakeUpConfigs();

    // update power statistics only when WiFi isn't used - fixing:
    // https://github.com/Open-Smartwatch/open-smartwatch-os/issues/163
    bool wifiDisabled = true;
#ifdef OSW_FEATURE_WIFI
    wifiDisabled = !OswServiceAllTasks::wifi.isEnabled();
#endif
    if(!wifiDisabled)
        return; // do not continue if wifi is enabled

    if(this->isCharging())
        return;
#ifdef OSW_FEATURE_WIFI
    if(OswServiceAllTasks::wifi.isEnabled())
        return;
#endif
    // TODO These updates do not respect battery degradation (or improvement by swapping) over time, you may add this :)
    if (currBattery < this->getBatteryRawMin() && currBattery > 10) {
        OSW_LOG_D("Updated minimum battery value to: ", currBattery);
        this->powerPreferences.putUShort("-", currBattery);
    }
    if (currBattery > this->getBatteryRawMax() && currBattery < 80) {
        OSW_LOG_D("Updated maximum battery value to: ", currBattery);
        this->powerPreferences.putUShort("+", currBattery);
    }
}

bool OswHal::isCharging() {
#if OSW_DEVICE_TPS2115A_STATPWR != 0
    return digitalRead(OSW_DEVICE_TPS2115A_STATPWR); // != 0 means there is V(IN2) in use
#else
#if OSW_PLATFORM_IS_FLOW3R_BADGE == 1
    return ~this->readGpioExtender() & 0b00000100;
#else
    return false;
#endif
#endif
}

/**
 * Reports the average of numAvg subsequent measurements
 */
uint16_t OswHal::getBatteryRaw(const uint16_t numAvg) {
#if OSW_DEVICE_ESP32_BATLVL != 0
    uint16_t b = 0;
    for (uint8_t i = 0; i < numAvg; i++)
        b = b + analogRead(OSW_DEVICE_ESP32_BATLVL);
    b = b / numAvg;
    return b > 40 ? b / 2 : b;
#else
    return 0; // TODO we should properly report no battery information available
#endif
}

/**
 * Uses power statistics min/max and a non-linear transformation curve
 * @return  [0,100]
 */
uint8_t OswHal::getBatteryPercent(void) {
    const uint16_t batRaw = this->getBatteryRaw();

#ifdef BATT_ASIGMOIDAL
    return asigmoidal(batRaw, this->getBatteryRawMin(), this->getBatteryRawMax());
#elif BATT_SIGMOIDAL
    return sigmoidal(batRaw, this->getBatteryRawMin(), this->getBatteryRawMax());
#elif BATT_LINEAR
    return linear(batRaw, this->getBatteryRawMin(), this->getBatteryRawMax());
#else // BATT_LEGACY

    // https://en.wikipedia.org/wiki/Logistic_function
    // f(x)=L/(1+e(-k(x-x0)))
    // The value for k (=12) is chosen by guessing, just make sure f(0) < 0.5 to indicate the calibration process...
    // Original Formula: 1/(1+e^(-12*(x-0.5))*((1/0.5)-1))
    // Optimized Formula: 1/(1+e^(-12*(x-0.5)))

    const float minMaxDiff = (float) max(abs(this->getBatteryRawMax() - this->getBatteryRawMin()), 1); // To prevent division by zero
    const float batNormalized = ((float) batRaw - (float) this->getBatteryRawMin()) * (1.0f / minMaxDiff);
    const float batTransformed = 1.0f / (1 + powf(2.71828f, -12 * (batNormalized - 0.5f)));

    // Just in case here is a bug ;)
    // OSW_LOG_D("r", batRaw,
    //     "–", this->getBatteryRawMin(),
    //     "+", this->getBatteryRawMax(),
    //     "d", minMaxDiff,
    //     "n", batNormalized,
    //     "t", batTransformed);

    return max(min((uint8_t) roundf(batTransformed * 100), (uint8_t) 100), (uint8_t) 0);
#endif
}

// float OswHal::getBatteryVoltage(void) {
//   adcAttachPin(OSW_DEVICE_ESP32_BATLVL);
//   adcStart(B_OSW_DEVICE_ESP32_BATLVLMON);

//   // adc2_config_width(ADC_WIDTH_12Bit);
//   adc2_config_channel_atten(ADC2_CHANNEL_8, ADC_ATTEN_11db);
//   esp_adc_cal_characteristics_t characteristics;
//   esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_11, ADC_WIDTH_10Bit, V_REF, &characteristics);
//   uint32_t voltage = 0;
//   esp_adc_cal_get_voltage(ADC_CHANNEL_8, &characteristics, &voltage);

//   // some dodgy math to get a representable value
//   return voltage / (100.0f) + 0.3f;
// }

void OswHal::setCPUClock(uint8_t mhz) {
    // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-cpu.h
    // ESP32 PICO D4 -> https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/get-started-pico-kit.html
    // -> 40MHz Oscillator
    // //  240, 160, 80, 40, 20, 10  <<< For 40MHz XTAL
    setCpuFrequencyMhz(mhz);
}

uint8_t OswHal::getCPUClock() {
    return getCpuFrequencyMhz();
}

void OswHal::doSleep(bool deepSleep) {
#if OSW_PLATFORM_BLOCK_SLEEP == 1
    OSW_LOG_I("Sleeping is blocked by OSW_PLATFORM_BLOCK_SLEEP.");
    this->noteUserInteraction(); // reset sleep timer
    return;
#endif
    this->stop(!deepSleep);

    // register user wakeup sources
    if (OswConfigAllKeys::buttonToWakeEnabled.get())
        // or set Button1 wakeup if no sensor wakeups registered
        esp_sleep_enable_ext0_wakeup(GPIO_NUM_0 /* BTN_0 */, LOW); // special handling as low is the trigger, otherwise ↓ bitmask should be used!

    /**
     * Okay. Hear me out: In the very special case that you do not enable "button to wake" and only try to use the
     * "raise to wake / tap" feature the call to "esp_sleep_enable_ext1_wakeup()" somehow breaks the display rendering
     * after a deep sleep / watch reset. Meaning it will turn on, but no pixel is drawn. Idk know why, especially
     * everything works fine with both features enabled. Therefore I just readded a dirty "special-case" to do it the
     * old way and reuse the ext0_wakeup slot. I case you have an idea, why this is a problem... Fix it! Please.
     */
    if (OswConfigAllKeys::raiseToWakeEnabled.get() || OswConfigAllKeys::tapToWakeEnabled.get()) {
        if (!OswConfigAllKeys::buttonToWakeEnabled.get()) {
            esp_sleep_enable_ext0_wakeup(GPIO_NUM_34 /* BTN_1 */, HIGH);
        } else {
            esp_sleep_enable_ext1_wakeup(0x400000000 /* BTN_1 = GPIO_34 = 2^34 as bitmask */, ESP_EXT1_WAKEUP_ANY_HIGH);
        }
    }

    // register timer wakeup sources
    OswHal::WakeUpConfig* wakeupcfg = this->selectWakeUpConfig();
    this->persistWakeUpConfig(wakeupcfg, !deepSleep);
    if(wakeupcfg and wakeupcfg->time > time(nullptr)) {
        time_t seconds = wakeupcfg->time - time(nullptr);
        OSW_LOG_D("Wakeup configuration selected - see you in ", seconds, " seconds.");
        if(wakeupcfg->selected)
            wakeupcfg->selected();
        esp_err_t res = esp_sleep_enable_timer_wakeup(seconds * 1000 * 1000);
        if(res != ESP_OK)
            OSW_LOG_E("Error while setting up wakeup timer: ", res);
    }

    delay(100); // Make sure the Serial is flushed and any tasks are finished...
    if (deepSleep)
        esp_deep_sleep_start();
    else
        esp_light_sleep_start();
}

void OswHal::deepSleep() {
    doSleep(true);
}

void OswHal::lightSleep() {
    if(!OswConfigAllKeys::lightSleepEnabled.get()) {
        // The light sleep was not enabled, ignore this request and go to deep sleep instead!
        OSW_LOG_D("Request for light sleep ignored, as only deep sleep is enabled.");
        doSleep(true);
    } else {
        _isLightSleep = true;
        doSleep(false);
    }
}

void OswHal::handleWakeupFromLightSleep(void) {
    if (_isLightSleep) {
        // is there a better way to detect light sleep wakeups?
        _isLightSleep = false;
        this->setup(true);
    }
}

size_t OswHal::addWakeUpConfig(const WakeUpConfig& config) {
    std::lock_guard<std::mutex> lock(this->_wakeUpConfigsMutex);
    this->_wakeUpConfigs.push_back(config);
    this->_wakeUpConfigs.back().id = this->_wakeUpConfigIdCounter++;
    return this->_wakeUpConfigs.back().id;
}

void OswHal::removeWakeUpConfig(size_t configId) {
    std::lock_guard<std::mutex> lock(this->_wakeUpConfigsMutex);
    for(auto it = this->_wakeUpConfigs.begin(); it != this->_wakeUpConfigs.end(); ++it) {
        if(it->id == configId) {
            this->_wakeUpConfigs.erase(it);
            return;
        }
    }
}

void OswHal::expireWakeUpConfigs() {
    std::lock_guard<std::mutex> lock(this->_wakeUpConfigsMutex);
    for(auto it = this->_wakeUpConfigs.begin(); it != this->_wakeUpConfigs.end(); ) {
        if(it->time < time(nullptr)) {
            this->_wakeUpConfigsMutex.unlock(); // unlock before callback to avoid deadlocks
            if(it->expired) {
                OSW_LOG_D("Expired wakeup configuration found, calling callback...");
                it->expired();
            }
            this->_wakeUpConfigsMutex.lock();
            it = this->_wakeUpConfigs.erase(it);
        } else
            ++it;
    }
}

OswHal::WakeUpConfig* OswHal::selectWakeUpConfig() {
    std::lock_guard<std::mutex> lock(this->_wakeUpConfigsMutex);
    WakeUpConfig* selected = nullptr;
    for(auto it = this->_wakeUpConfigs.begin(); it != this->_wakeUpConfigs.end(); ++it) {
        if(!selected || it->time < selected->time)
            selected = &(*it);
    }
    return selected;
}

void OswHal::resetWakeUpConfig(bool useLightSleep) {
    if(useLightSleep)
        this->_lightSleepWakeUpConfig = nullptr;
    else
        this->powerPreferences.remove("cfg"); // Ignore if we can't remove it - maybe it was not there in the first place
}

void OswHal::persistWakeUpConfig(OswHal::WakeUpConfig* config, bool toLightSleep) {
    if(config) {
        if(toLightSleep) {
            // In this case just store it in memory to reduce write load on the flash
            this->_lightSleepWakeUpConfig = config;
        } else {
            size_t written = this->powerPreferences.putBytes("cfg", config, sizeof(WakeUpConfig));
            assert(written == sizeof(WakeUpConfig) && "Could not write wakeup config to preferences!");
        }
    } else
        this->resetWakeUpConfig(toLightSleep);
}

std::optional<OswHal::WakeUpConfig> OswHal::readAndResetWakeUpConfig(bool fromLightSleep) {
    WakeUpConfig config;
    if(fromLightSleep) {
        if(this->_lightSleepWakeUpConfig)
            config = *this->_lightSleepWakeUpConfig;
        else
            return std::nullopt;
    } else {
        size_t read = this->powerPreferences.getBytes("cfg", &config, sizeof(WakeUpConfig));
        if(read != sizeof(WakeUpConfig))
            return std::nullopt;
    }
    this->resetWakeUpConfig(fromLightSleep);
    return config;
}

void OswHal::noteUserInteraction() {
    this->_lastUserInteraction = millis();
}

void OswHal::handleDisplayTimout() {
    // Did enough time pass since the last user interaction?
    const int lastDisplayTimeout = OswConfigAllKeys::settingDisplayTimeout.get();
    if (lastDisplayTimeout == 0 or this->_lastUserInteraction + lastDisplayTimeout * 1000 > millis())
        return;
    // Does the UI allow us to go to sleep?
    OswAppV2* app = OswUI::getInstance()->getRootApplication();
    if(app == nullptr or app->getViewFlags() & OswAppV2::ViewFlags::KEEP_DISPLAY_ON)
        return;
    this->lightSleep();
    this->noteUserInteraction(); // Oh, we came back from sleep, so we are "interacting" with the watch
}
