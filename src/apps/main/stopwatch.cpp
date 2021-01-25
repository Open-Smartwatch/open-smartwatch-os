
#include "./apps/main/stopwatch.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

// continue after sleep does not work yet
// because millis restarts from 0
// TODO: use the I2C RTC instead
RTC_DATA_ATTR long start = 0;
RTC_DATA_ATTR long diff = 0;
RTC_DATA_ATTR bool running = false;
RTC_DATA_ATTR bool reset = true;

// OswAppHelloWorld::OswAppHelloWorld(void) : OswApp() {}
void OswAppStopWatch::setup(OswHal* hal) {}

// TODO: move the next three functions to library
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
  // Start
  if (hal->btn3Down()) {
    if (reset) {
      start = hal->getTime();
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
  if (reset) {
    hal->getCanvas()->setCursor(220 - defaultFontXOffset(5, 2), 42);
    hal->getCanvas()->print("Start");
  } else if (!running) {
    hal->getCanvas()->setCursor(220 - defaultFontXOffset(8, 2), 42);
    hal->getCanvas()->print("Continue");
  }

  if (running) {
    hal->getCanvas()->setCursor(220 - defaultFontXOffset(4, 2), 182);
    hal->getCanvas()->print("Stop");
  } else if (!reset) {
    hal->getCanvas()->setCursor(220 - defaultFontXOffset(5, 2), 182);
    hal->getCanvas()->print("Reset");
  }

  if (running) {
    diff = hal->getTime() - start;
  }

  long deltaSeconds = (diff) % 60;
  long deltaMinutes = (diff / 60) % 60;
  long deltaHours = (diff / 60 / 60) % 24;
  long deltaDays = diff / 60 / 60 / 24;

  hal->getCanvas()->setTextSize(4);

  if (deltaDays) {
    hal->getCanvas()->setCursor(120 - defaultFontXOffset(deltaDays < 10 ? 1 : 2, 4),  //
                                120 - defaultFontYOffset(1, 4) * 1.5);
    hal->getCanvas()->print(deltaDays);
    hal->getCanvas()->print("d");
  }

  hal->getCanvas()->setCursor(defaultFontXOffset(1, 4), 120 - defaultFontYOffset(1, 4) / 2);
  print2Digits(hal, deltaHours);
  hal->getCanvas()->print(":");
  print2Digits(hal, deltaMinutes);
  hal->getCanvas()->print(":");
  print2Digits(hal, deltaSeconds);
  // hal->getCanvas()->print(".");
  // pushing the button is inaccurate
  // also we have more space on the screen this way
  // hal->getCanvas()->print(deltaMillis / 100);

  hal->requestFlush();
}

void OswAppStopWatch::stop(OswHal* hal) {}
