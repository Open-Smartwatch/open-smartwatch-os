
#include "osw_hal.h"
#include "osw_pins.h"

// Graphics2D screenBuffer(DISP_W, DISP_H, DISP_CHUNK_H);

void OswHal::setupButtons(void) {
  pinMode(BTN_1, INPUT);
  pinMode(BTN_2, INPUT);
  pinMode(BTN_3, INPUT);
}
