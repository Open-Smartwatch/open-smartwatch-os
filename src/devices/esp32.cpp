#include <ctime> // The native ESP32 clock is wrapped by the standard time header
#include <cstdlib>
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

// Following forward declaration to access the internal temperature sensor of the ESP32, which is may not built-in (-> https://gist.github.com/xxlukas42/7e7e18604f61529b8398f7fcc5785251?permalink_comment_id=3532040#gistcomment-3532040)...
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
#if OSW_DEVICE_ESP32_USE_INTTEMP == 1
    for(int i = 0; i < 10; i++)
        if(temprature_sens_read() == 128)
            this->tempSensorIsBuiltIn = false;
#else
    this->tempSensorIsBuiltIn = false;
#endif
    setenv("TZ", "UTC0", 1); // Force systems clock to correspond to UTC
}

void OswDevices::NativeESP32::update() {
    if(this->enableTimeResync) {
        // The clock of the ESP32 sometimes drifts very rapidly. This checks for an other available provider
        // and resyncs the ESP32 clock with it (as this is only way to control which time is reported by
        // the function "time(nullptr);")...
        time_t nowEsp = this->getUTCTime();
        time_t nowOther = OswHal::getInstance()->getUTCTime(); // In the future maybe respect priorities?
        const time_t maxDiff = 4;
        if(abs(nowEsp - nowOther) > maxDiff) {
            // Oh, the ESP is async again - resync!
            this->setUTCTime(nowOther);
            OSW_LOG_D("Resynced internal ESP32 clock with other provider due to significant time difference (> ", maxDiff, " seconds).");
        }
    }
}

time_t OswDevices::NativeESP32::getUTCTime() {
    return time(nullptr);
}

void OswDevices::NativeESP32::setUTCTime(const time_t& epoch) {
    struct timeval now = {
        .tv_sec = epoch,
        .tv_usec = 0
    };
    settimeofday(&now, nullptr);
}

/**
 * @brief Works as described in OswTimeProvider - but this implementation
 * may change the output of the function std::localtime() temporarly!
 *
 * @throws std::logic_error if the provider does not support timezones
 * @param timestamp timestamp to transform
 * @param timezone based on this POSIX string
 * @return time_t transformed timestamp
 */
time_t OswDevices::NativeESP32::getTimezoneOffset(const time_t& timestamp, const String& timezone) {
    bool hasOldTimezone = getenv("TZ") != nullptr;
    String oldTimezone; // Variable to hold local copy, as the value by getenv() may change after a setenv()
    if(hasOldTimezone)
        oldTimezone = getenv("TZ");

    setenv("TZ", timezone.c_str(), 1); // overwrite the TZ environment variable
    tzset();
    std::tm localTm = *std::localtime(&timestamp);

    setenv("TZ", "UTC0", 1); // overwrite the TZ environment variable
    tzset();
    std::tm utcTm = *std::localtime(&timestamp);
    std::time_t utc = std::mktime(&utcTm);
    std::time_t local = std::mktime(&localTm); // this removes the "local"/dst offsets by UTC from our localized timestamp -> we get the UTC timestamp INCLUDING the local offsets!

    // Why not just use this ↓ ? Because the code below works with GMT to UTC and also
    // gives the mktime() a non-localozed timestamp, causing the DST to be dropped.
    // Therefore it works great in the winter, but breaks in the summer (...).
    // setenv("TZ", timezone.c_str(), 1); // overwrite the TZ environment variable
    // tzset();
    // const time_t utc = mktime(std::gmtime(&timestamp));
    // const time_t local = mktime(std::localtime(&timestamp));

    if(hasOldTimezone) {
        setenv("TZ", oldTimezone.c_str(), 1); // restore the TZ environment variable
        tzset();
    }
    if(utc == -1 or local == -1)
        throw std::logic_error("Could not represent times in std::time_t?!");
    return local - utc;
}

float OswDevices::NativeESP32::getTemperature() {
#if OSW_DEVICE_ESP32_USE_INTTEMP == 1
    const uint8_t temp = temprature_sens_read();
    if(!this->tempSensorIsBuiltIn)
        return 0.0f;
    return (temp - 32) / 1.8f;
#else
    return 0.0f;
#endif
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
    this->waitingForNTP = true;
    this->setClockResyncEnabled(false); // Do not try to resync with the other time providers, as this one is waiting for the NTP response...

#ifndef OSW_EMULATOR
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
#else
    OSW_EMULATOR_THIS_IS_NOT_IMPLEMENTED;
#endif

    OSW_LOG_D("[NTP] Started update...");
}

/**
 * @brief After an NTP update was triggered, check if it was successful and set the time. This will also update or reset the internal state of e.g. the time resync.
 *
 * @return true Only ONCE after the NTP update was successful
 * @return false
 */
bool OswDevices::NativeESP32::checkNTPUpdate() {
    if (!this->waitingForNTP or this->getUTCTime() < this->successfulNTPTime)
        return false; // NTP not yet updated
    this->setClockResyncEnabled(true); // Someone had information about the current time and shared it with us -> enable resync
    this->waitingForNTP = false;
    OSW_LOG_D("[NTP] Update finished (time of ", this->getUTCTime(), ")!");
    return true;
}

void OswDevices::NativeESP32::setClockResyncEnabled(const bool& enable) {
    if(this->enableTimeResync == enable)
        return;
    if(!this->enableTimeResync)
        OSW_LOG_D("Enabled time resync with primary provider.");
    else
        OSW_LOG_D("Disabled time resync with primary provider.");
    this->enableTimeResync = enable;
}

bool OswDevices::NativeESP32::isClockResyncEnabled() {
    return this->enableTimeResync;
}
