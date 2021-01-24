
#include "./apps/main/stopwatch.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

// OswAppHelloWorld::OswAppHelloWorld(void) : OswApp() {}
void OswAppStopWatch::setup(OswHal* hal) {}

void print2Digits(OswHal* hal, long n) {
  if (n < 10) {
    hal->getCanvas()->print("0");
  }
  hal->getCanvas()->print(n);
}

uint16_t defaultFontXOffset(uint16_t numChars, uint16_t scale) {  // works with defaut font only
  return numChars * 6 * scale;
}
uint16_t defaultFontYOffset(uint16_t numRows, uint16_t scale) {  // works with defaut font only
  return numRows * 8 * scale;
}

void OswAppStopWatch::loop(OswHal* hal) {
  static long start = 0;
  static long diff = 0;
  static bool running = false;
  static bool reset = true;

  // Start
  if (hal->btn3Down()) {
    if (reset) {
      start = millis();
    }
    running = true;
    reset = false;
  }

  // Stop -> Reset
  if (hal->btn2Down()) {
    if (running) {
      running = false;
    } else {
      diff = 0;
      reset = true;
    }
  }

  hal->getCanvas()->fillScreen(0);
  hal->getCanvas()->setTextColor(rgb565(255, 255, 255));

  hal->getCanvas()->setTextSize(2);
  hal->getCanvas()->setCursor(220 - defaultFontXOffset(5, 2), 42);
  hal->getCanvas()->print("Start");
  hal->getCanvas()->setCursor(220 - defaultFontXOffset(10, 2), 182);
  hal->getCanvas()->print("Stop/Reset");

  if (running) {
    diff = millis() - start;
  }

  long deltaMillis = diff % 1000;
  long deltaSeconds = (diff / 1000) % 60;
  long deltaMinutes = (diff / 1000 / 60) % 60;
  long deltaHours = (diff / 1000 / 60 / 60) % 24;
  long deltaDays = diff / 1000 / 60 / 60 / 24;

  hal->getCanvas()->setTextSize(4);

  if (deltaDays) {
    hal->getCanvas()->setCursor(120 - defaultFontXOffset(deltaDays < 10 ? 1 : 2, 4),  //
                                120 - defaultFontYOffset(1, 4) * 1.5);
    hal->getCanvas()->print(deltaDays);
    hal->getCanvas()->print("d");
  }

  hal->getCanvas()->setCursor(0, 120 - defaultFontYOffset(1, 4) / 2);
  print2Digits(hal, deltaHours);
  hal->getCanvas()->print(":");
  print2Digits(hal, deltaMinutes);
  hal->getCanvas()->print(":");
  print2Digits(hal, deltaSeconds);
  hal->getCanvas()->print(".");
  // pushing the button is inaccurate
  // also we have more space on the screen this way
  hal->getCanvas()->print(deltaMillis / 100);

  hal->requestFlush();
}

void OswAppStopWatch::stop(OswHal* hal) {}
