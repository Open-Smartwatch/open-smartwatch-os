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
  pinMode(STAT_PWR, INPUT);
  pinMode(B_MON, INPUT);
  powerStatistics.begin("osw-power", false);
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
    #ifdef DEBUG
    Serial.print(String(__FILE__) + ": Updated minimum battery value to: ");
    Serial.println(currBattery);
    #endif
    this->powerStatistics.putUShort("-", currBattery);
  }
  if (currBattery > this->getBatteryRawMax() && currBattery < 80) {
    #ifdef DEBUG
    Serial.print(String(__FILE__) + ": Updated maximum battery value to: ");
    Serial.println(currBattery);
    #endif
    this->powerStatistics.putUShort("+", currBattery);
  }
}

boolean OswHal::isCharging(void) {
  return digitalRead(STAT_PWR); // != 0 means there is V(IN2) in use
}

/**
 * Reports the average of numAvg subsequent measurements
 */
uint16_t OswHal::getBatteryRaw(const uint16_t numAvg) {
  uint16_t b = 0;
  for (uint8_t i = 0; i < numAvg; i++)
    b = b + analogRead(B_MON);
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
//   adcAttachPin(B_MON);
//   adcStart(B_MON);

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

void doSleep(bool deepSleep, bool wakeFromButtonOnly = false, long millis = 0) {
  // turn off gps (this needs to be able to prohibited by app)
#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)
  OswHal::getInstance()->gpsBackupMode();
  OswHal::getInstance()->sdOff();
#endif

  // turn off screen
  OswHal::getInstance()->displayOff();

  // register user wakeup sources
  if (wakeFromButtonOnly || // force button wakeup
      (!OswConfigAllKeys::raiseToWakeEnabled.get() && !OswConfigAllKeys::tapToWakeEnabled.get())) {
    // ore set Button1 wakeup if no sensor wakeups registered
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0 /* BTN_0 */, LOW);
  } else if (OswConfigAllKeys::raiseToWakeEnabled.get() || OswConfigAllKeys::tapToWakeEnabled.get()) {
    // tilt to wake and tap interrupts are configured in sensors.setup.
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_34 /* BMA_INT_1 */, HIGH);
  }

  // register timer wakeup sources
  if (millis) {
#ifdef DEBUG
    Serial.print("-> wake up in millis: ");
    Serial.println(millis);
#endif
    esp_sleep_enable_timer_wakeup(millis * 1000);
  }

  if (deepSleep) {
#ifdef DEBUG
    Serial.println("-> deep sleep ");
#endif
    esp_deep_sleep_start();
  } else {
#ifdef DEBUG
    Serial.println("-> light sleep ");
#endif
    esp_light_sleep_start();
  }
}

void OswHal::deepSleep(long millis, bool wakeFromButtonOnly) { doSleep(true, wakeFromButtonOnly, millis); }

void OswHal::lightSleep() {
  _isLightSleep = true;
  doSleep(false);
}

void OswHal::lightSleep(long millis) {
  _isLightSleep = true;
  doSleep(false, false, millis);
}

void OswHal::handleWakeupFromLightSleep(void) {
  if (_isLightSleep) {
    // is there a better way to detect light sleep wakeups?
    _isLightSleep = false;
    displayOn();
    setBrightness(OswConfigAllKeys::settingDisplayBrightness.get());
  }
}
