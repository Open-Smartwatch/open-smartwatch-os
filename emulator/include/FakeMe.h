#pragma once
// #include <stdint.h>
// #include <math.h>

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
void esp_light_sleep_start();