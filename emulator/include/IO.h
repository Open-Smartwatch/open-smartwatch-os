#pragma once

#include "DataTypes.h"

#define INPUT 0
#define OUTPUT 0

#define LOW 0
#define HIGH 255

#define GPIO_NUM_0 255 // FIX THIS!!!
#define GPIO_NUM_34 255 // FIX THIS!!!

#define esp_sleep_enable_ext0_wakeup(...)
#define esp_sleep_enable_ext1_wakeup(...)
#define esp_sleep_enable_timer_wakeup(...)

void pinMode(int, int);
uint8_t digitalRead(int);
uint8_t analogRead(int);
void digitalWrite(int, uint8_t);
void ledcWrite(int, int);
void esp_deep_sleep_start();
void esp_light_sleep_start();