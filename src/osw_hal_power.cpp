#include "osw_hal.h"
#include "osw_pins.h"

void OswHal::setupPower(void) {
  pinMode(STAT_PWR, INPUT);
  pinMode(B_MON, INPUT);
}