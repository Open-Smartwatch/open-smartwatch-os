#include <time.h> // The native ESP32 clock is wrapped by the standard time header
#include <sys/cdefs.h>
#include <sys/time.h>
#ifndef OSW_EMULATOR
#include <esp32-hal.h>
#endif

#include <Wire.h>
#include <string>

#include <osw_hal.h>
#include <osw_config_keys.h>
#include <devices/esp32.h>

// Following forward declaration to access the interal temperature sensor of the ESP32, which is may not built-in (-> https://gist.github.com/xxlukas42/7e7e18604f61529b8398f7fcc5785251?permalink_comment_id=3532040#gistcomment-3532040)...
#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

#ifdef OSW_EMULATOR
uint8_t temprature_sens_read() {
    return 128;
}
#endif

void OswDevices::NativeESP32::setup() {
    // Test temperature for 128 (sensor not available) for 10 times
    for(int i = 0; i < 10; i++)
        if(temprature_sens_read() == 128)
            this->tempSensorIsBuiltIn = false;
}

void OswDevices::NativeESP32::update() {
    // The clock of the ESP32 sometimes drifts very rapidly. This checks for an other available provider
    // and resyncs the ESP32 clock with it (as this is only way to control which time is reported by
    // the function "time(nullptr);")...
    time_t nowEsp = this->getUTCTime();
    time_t nowOther = OswHal::getInstance()->getUTCTime(); // In the future maybe respect priorities?
    const time_t maxDiff = 4;
    if(abs(nowEsp - nowOther) > maxDiff) {
        // Oh, the ESP is async again - resync!
        this->setUTCTime(nowOther);
#ifndef NDEBUG
        Serial.println(String(__FILE__) + ": Resynced internal ESP32 clock with other provider due to significant time difference (> " + String(maxDiff) + " seconds).");
#endif
    }
}

time_t OswDevices::NativeESP32::getUTCTime() {
    return time(nullptr);
};

void OswDevices::NativeESP32::setUTCTime(const time_t& epoch) {
    struct timeval now = {
        .tv_sec = epoch,
        .tv_usec = 0
    };
    settimeofday(&now, nullptr);
};

float OswDevices::NativeESP32::getTemperature() {
    const uint8_t temp = temprature_sens_read();
    if(!this->tempSensorIsBuiltIn)
        return 0.0f;
    return (temp - 32) / 1.8;
}

bool OswDevices::NativeESP32::isTemperatureSensorAvailable() {
    return this->tempSensorIsBuiltIn;
}

/**
 * This starts a NTP client to reteive the current time. This will also set the timezone of the internal RTC.
 * This this will ALSO SET the current RTC to 0, as long as the NTP update is not finished...
 */
void OswDevices::NativeESP32::triggerNTPUpdate() {
    this->setUTCTime(0);
#ifndef OSW_EMULATOR
    configTime(OswConfigAllKeys::timeZone.get() * 3600 + 3600, OswConfigAllKeys::daylightOffset.get() * 3600, "pool.ntp.org", "time.nist.gov");
#else
    //TODO
#endif
}