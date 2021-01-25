#include "./overlays/overlays.h"

void drawUsbConnected(OswHal* hal) {
  hal->getCanvas()->getGraphics2D()->fillFrame(104, 10, 13, 2, rgb565(128, 128, 128));  // cable dot
  hal->getCanvas()->getGraphics2D()->fillFrame(117, 8, 3, 6, rgb565(200, 200, 200));    // cable to casing
  hal->getCanvas()->getGraphics2D()->fillFrame(124, 8, 11, 6, rgb565(128, 128, 128));   // connector
  hal->getCanvas()->getGraphics2D()->fillFrame(120, 6, 8, 10, rgb565(200, 200, 200));   // casing
}

void drawOverlays(OswHal* hal) {

    if (hal->isCharging()) {
        drawUsbConnected(hal);
    }
}