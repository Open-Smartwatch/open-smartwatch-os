#pragma once

#include "DataTypes.h"

#define INPUT 0
#define OUTPUT 0

#define LOW 0
#define HIGH 255

#define GPIO_NUM_0 255 // TODO FIX THIS!!!
#define GPIO_NUM_34 255 // TODO FIX THIS!!!

void pinMode(int, int);
uint8_t digitalRead(int);
uint8_t analogRead(int);
void digitalWrite(int, uint8_t);
void ledcWrite(int, int);