#include <esp_adc_cal.h>

#include "driver/rtc_io.h"
#include "osw_hal.h"
#include "osw_pins.h"

#define V_REF 1100  // ADC reference voltage
#define CHANNEL ADC2_CHANNEL_8

void OswHal::setupPower(void) {
  pinMode(STAT_PWR, INPUT);
  pinMode(B_MON, INPUT);
  pinMode(MODE_SYNC, OUTPUT);

  digitalWrite(MODE_SYNC, LOW);
}

boolean OswHal::isCharging(void) {
  return digitalRead(STAT_PWR);  // TODO: check if 1 = charging or not :)
}

float OswHal::getBatteryVoltage(void) {
  adcAttachPin(B_MON);
  adcStart(B_MON);

  // adc2_config_width(ADC_WIDTH_12Bit);
  adc2_config_channel_atten(ADC2_CHANNEL_8, ADC_ATTEN_11db);
  esp_adc_cal_characteristics_t characteristics;
  esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_11, ADC_WIDTH_10Bit, V_REF, &characteristics);
  uint32_t voltage = 0;
  esp_adc_cal_get_voltage(ADC_CHANNEL_8, &characteristics, &voltage);

  float r1 = 200000;
  float r2 = 10000;
  // // ESP32: analogRead returns a value 0-4095
  // float in = ((float)adcEnd(B_MON)) / 4096.0;
  // float vRef = 3.3;  // V
  // return vRef * in * ((r1 + r2) / r2)

  // first from millis to volt, then by voltage divider factor
  return voltage / (1000.0);
}

void OswHal::setCPUClock(uint8_t mhz) {
  // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-cpu.h
  // ESP32 PICO D4 -> https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/get-started-pico-kit.html
  // -> 40MHz Oscillator
  // //  240, 160, 80, 40, 20, 10  <<< For 40MHz XTAL
  setCpuFrequencyMhz(mhz);
}

void OswHal::deepSleep(void) {
  this->gpsBackupMode();
  this->setBrightness(0);
  this->displayOff();
  this->sdOff();

  rtc_gpio_isolate(GPIO_NUM_5);
  rtc_gpio_isolate(GPIO_NUM_9);
  rtc_gpio_isolate(GPIO_NUM_10);
  rtc_gpio_isolate(GPIO_NUM_12);
  rtc_gpio_isolate(GPIO_NUM_13);
  rtc_gpio_isolate(GPIO_NUM_15);
  rtc_gpio_isolate(GPIO_NUM_18);
  rtc_gpio_isolate(GPIO_NUM_21);
  rtc_gpio_isolate(GPIO_NUM_22);
  rtc_gpio_isolate(GPIO_NUM_23);
  rtc_gpio_isolate(GPIO_NUM_25);
  rtc_gpio_isolate(GPIO_NUM_27);
  rtc_gpio_isolate(GPIO_NUM_32);
  rtc_gpio_isolate(GPIO_NUM_33);
  rtc_gpio_isolate(GPIO_NUM_34);
  rtc_gpio_isolate(GPIO_NUM_35);

  esp_deep_sleep_start();
};