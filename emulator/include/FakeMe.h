#pragma once

#include "Defines.h"

#include "DataTypes.h"
#include "String.h"
#include "Serial.h"
#include "IO.h"
#include "Display.h"
#include "CPU.h"
#include "ESP.h"

void sleep(int);
int rtc_get_reset_reason(int);
void randomSeed(long l);
void esp_deep_sleep_start();
esp_err_t esp_light_sleep_start();
esp_err_t esp_sleep_enable_timer_wakeup(unsigned long time_in_us);
esp_err_t esp_sleep_enable_ext0_wakeup(unsigned long gpio_num, int level);
esp_err_t esp_sleep_enable_ext1_wakeup(unsigned long gpio_num, int level);