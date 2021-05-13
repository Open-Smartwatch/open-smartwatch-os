#ifndef _TONE32_h
#define _TONE32_h

#define TONE_CHANNEL 15
#include "Arduino.h"
#include "pitches.h"

void tone(uint8_t pin, unsigned int frequency, unsigned long duration = 0, uint8_t channel = TONE_CHANNEL);
void noTone(uint8_t pin, uint8_t channel = TONE_CHANNEL);
#endif



