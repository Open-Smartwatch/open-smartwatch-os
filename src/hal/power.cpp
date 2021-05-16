#include <esp_adc_cal.h>

#include "driver/rtc_io.h"
#include "osw_hal.h"
#include "osw_pins.h"

#define V_REF 1100  // ADC reference voltage
#define CHANNEL ADC2_CHANNEL_8

void OswHal::setupPower(void) {
  pinMode(STAT_PWR, INPUT);
  pinMode(B_MON, INPUT);
}

boolean OswHal::isCharging(void) {
  return digitalRead(STAT_PWR);  // TODO: check if 1 = charging or not :)
}

uint16_t OswHal::getBatteryRaw(void) {
  pinMode(B_MON, INPUT);
  return analogRead(B_MON);
}

uint8_t OswHal::getBatteryPercent(void) {
  uint16_t b = 0;

  uint8_t n = 20;
  // measure n times
  for (uint8_t i = 0; i < n; i++) {
    b = b + getBatteryRaw();
  }
  b = b / n;

  b = b > 40 ? b / 2 : b;

  // magic values through a single experiment:
  if (b >= 31) {
    return 100;
  } else if (b >= 30) {
    return 80;
  } else if (b >= 28) {
    return 60;
  } else if (b >= 27) {
    return 40;
  } else if (b >= 26) {
    return 20;
  } else if (b >= 25) {
    return 10;
  } else if (b >= 20) {
    return 5;
  } else {
    return 0;
  }
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
void OswHal::deepSleep() {
  printf("Going to sleep\n");

  this->setBrightness(0);
  this->displayOff();

#if defined(GPS_EDITION)
  this->gpsBackupMode();
  this->sdOff();
#endif

  // TODO: this is not optimal / implement this properly
  // rtc_gpio_isolate(GPIO_NUM_5);
  // rtc_gpio_isolate(GPIO_NUM_9);
  // rtc_gpio_isolate(GPIO_NUM_10);
  // rtc_gpio_isolate(GPIO_NUM_12);
  // rtc_gpio_isolate(GPIO_NUM_13);
  // rtc_gpio_isolate(GPIO_NUM_15);
  // rtc_gpio_isolate(GPIO_NUM_18);
  // rtc_gpio_isolate(GPIO_NUM_21);
  // rtc_gpio_isolate(GPIO_NUM_22);
  // rtc_gpio_isolate(GPIO_NUM_23);
  // rtc_gpio_isolate(GPIO_NUM_25);
  // rtc_gpio_isolate(GPIO_NUM_27);
  // rtc_gpio_isolate(GPIO_NUM_32);
  // rtc_gpio_isolate(GPIO_NUM_33);
  // rtc_gpio_isolate(GPIO_NUM_34);
  // rtc_gpio_isolate(GPIO_NUM_35);
  // esp_sleep_enable_ext0_wakeup(GPIO_NUM_0 /* BTN_0 */, LOW);
  // esp_sleep_enable_ext0_wakeup(GPIO_NUM_35 /* BMA_INT_2 / TAP */, HIGH);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_34 /* BMA_INT_1 */, HIGH);


  esp_deep_sleep_start();
};

void OswHal::deepSleep(long millis) {
  esp_sleep_enable_timer_wakeup(millis * 1000);
  deepSleep();
};
