#include <esp_adc_cal.h>

#include "driver/rtc_io.h"
#include "osw_hal.h"
#include "osw_pins.h"

#include <services/OswServiceTaskWiFi.h>
#include <services/OswServiceTasks.h>

#define V_REF 1100  // ADC reference voltage
#define CHANNEL ADC2_CHANNEL_8

uint16_t OswHal::getBatteryRawMin() {
  return this->powerStatistics.getUShort("-", 60); // Every battery should be able to deliver lower than this at some point
}

uint16_t OswHal::getBatteryRawMax() {
  return this->powerStatistics.getUShort("+", 26); // Every battery should be able to deliver more than this
}

void OswHal::setupPower(void) {
  pinMode(OSW_DEVICE_TPS2115A_STATPWR, INPUT);
  pinMode(OSW_DEVICE_ESP32_BATLVL, INPUT);
  powerStatistics.begin("osw-power", false);
}

void OswHal::stopPower(void) {
  powerStatistics.end();
}

/**
 * Update the power statistics, ignores unrealistic battery values (value must be 10 < v < 80) and only works during dischrging and without wifi enabled (bug on current hardware revisions)
 */
void OswHal::updatePowerStatistics(uint16_t currBattery) {
  if(this->isCharging())
    return;
  #ifdef OSW_FEATURE_WIFI
  if(OswServiceAllTasks::wifi.isEnabled())
    return;
  #endif
  // TODO These updates do not respect battery degradation (or improvement by swapping) over time, you may add this :)
  if (currBattery < this->getBatteryRawMin() && currBattery > 10) {
    #ifndef NDEBUG
    Serial.print(String(__FILE__) + ": Updated minimum battery value to: ");
    Serial.println(currBattery);
    #endif
    this->powerStatistics.putUShort("-", currBattery);
  }
  if (currBattery > this->getBatteryRawMax() && currBattery < 80) {
    #ifndef NDEBUG
    Serial.print(String(__FILE__) + ": Updated maximum battery value to: ");
    Serial.println(currBattery);
    #endif
    this->powerStatistics.putUShort("+", currBattery);
  }
}

boolean OswHal::isCharging(void) {
  return digitalRead(OSW_DEVICE_TPS2115A_STATPWR); // != 0 means there is V(IN2) in use
}

/**
 * Reports the average of numAvg subsequent measurements
 */
uint16_t OswHal::getBatteryRaw(const uint16_t numAvg) {
  uint16_t b = 0;
  for (uint8_t i = 0; i < numAvg; i++)
    b = b + analogRead(OSW_DEVICE_ESP32_BATLVL);
  b = b / numAvg;
  return b > 40 ? b / 2 : b;
}

/**
 * Uses power statistics min/max and a non-linear transformation curve
 * @return  [0,100]
 */
uint8_t OswHal::getBatteryPercent(void) {
  const uint16_t batRaw = this->getBatteryRaw();

  // https://en.wikipedia.org/wiki/Logistic_function
  // The value for k (=12) is choosen by guessing, just make sure f(0) < 0.5 to indicate the calibration process...
  // Original Formula: 1/(1+e^(-12*(x-0.5))*((1/0.5)-1))
  // Optimized Formula: 1/(1+e^(-12*(x-0.5)))

  const float minMaxDiff = (float) max(abs(this->getBatteryRawMax() - this->getBatteryRawMin()), 1); // To prevent division by zero
  const float batNormalized = ((float) batRaw - (float) this->getBatteryRawMin()) * (1.0f / minMaxDiff);
  const float batTransformed = 1 / (1 + pow(2.71828, -12 * (batNormalized - 0.5)));


  // Just in case here is a bug ;)
  //Serial.print(__FILE__);
  //Serial.print(" r"); Serial.print(batRaw);
  //Serial.print("–"); Serial.print(this->getBatteryRawMin());
  //Serial.print("+"); Serial.print(this->getBatteryRawMax());
  //Serial.print("d"); Serial.print(minMaxDiff);
  //Serial.print("n"); Serial.print(batNormalized);
  //Serial.print("t"); Serial.println(batTransformed);


  return max(min((uint8_t) roundf(batTransformed * 100), (uint8_t) 100), (uint8_t) 0);
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
//   return voltage / (100.0) + 0.3;
// }

void OswHal::setCPUClock(uint8_t mhz) {
  // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-cpu.h
  // ESP32 PICO D4 -> https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/get-started-pico-kit.html
  // -> 40MHz Oscillator
  // //  240, 160, 80, 40, 20, 10  <<< For 40MHz XTAL
  setCpuFrequencyMhz(mhz);
}

void doSleep(bool deepSleep, long millis = 0) {
  OswHal::getInstance()->stop(!deepSleep);

  // register user wakeup sources
  if (OswConfigAllKeys::buttonToWakeEnabled.get())
    // ore set Button1 wakeup if no sensor wakeups registered
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
  if (millis) {
#ifndef NDEBUG
    Serial.print("-> wake up in millis: ");
    Serial.println(millis);
#endif
    esp_sleep_enable_timer_wakeup(millis * 1000);
  }

  if (deepSleep)
    esp_deep_sleep_start();
  else
    esp_light_sleep_start();
}

void OswHal::deepSleep(long millis) { doSleep(true, millis); }

void OswHal::lightSleep(long millis) {
  if(!OswConfigAllKeys::lightSleepEnabled.get()) {
    // The light sleep was not enabled, ignore this request and go to deep sleep instead!
#ifndef NDEBUG
    Serial.println(String(__FILE__) + " Request for light sleep ignored, as only deep sleep is enabled.");
#endif
    this->deepSleep(millis);
  } else {
    _isLightSleep = true;
    doSleep(false, millis);
  }
}

void OswHal::handleWakeupFromLightSleep(void) {
  if (_isLightSleep) {
    // is there a better way to detect light sleep wakeups?
    _isLightSleep = false;
    this->setup(true);
  }
}
