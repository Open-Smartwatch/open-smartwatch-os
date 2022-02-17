#pragma once

#include "Defines.h"

#include "DataTypes.h"
#include "String.h"
#include "Serial.h"
#include "Arduino_Canvas_Graphics2D.h"
#include "IO.h"
#include "Display.h"
#include "CPU.h"
#include "ESP.h"

void sleep(int);
int rtc_get_reset_reason(int);
void randomSeed(long l);