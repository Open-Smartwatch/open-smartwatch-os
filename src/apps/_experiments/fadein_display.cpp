
#include "./apps/_experiments/fadein_display.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

void OswAppFadeInDisplay::loop(OswHal* hal) {
  static long startup = 0;
  if (startup == 0) {
    startup = millis();
  }

  if (millis() - startup < _fadeDuration) {
    hal->setBrightness((millis() - startup) / (_fadeDuration / 255));
  }
}
