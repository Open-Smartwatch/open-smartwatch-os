
#include "./apps/main/stopwatch.h"

#include "gfx_util.h"
#include "osw_app.h"
#include "osw_hal.h"
#include "osw_ui_util.h"

#include "config.h" // Include the config here again to access the language definitions of it

// continue after sleep does not work yet
// because millis restarts from 0
// TODO: use the I2C RTC instead
RTC_DATA_ATTR long start = 0;
RTC_DATA_ATTR long diff = 0;
RTC_DATA_ATTR bool running = false;
RTC_DATA_ATTR bool reset = true;
RTC_DATA_ATTR long sumPaused = 0;

// OswAppHelloWorld::OswAppHelloWorld(void) : OswApp() {}
void OswAppStopWatch::setup(OswHal* hal) {
  // hal->enableDisplayBuffer();
}

void OswAppStopWatch::loop(OswHal* hal) {
  if (hal->btn3Down()) {
    if (reset) {  // Start
      start = hal->getLocalTime();
    } else {  // Continue
      sumPaused += diff;
      start = hal->getLocalTime();
    }
    running = true;
    reset = false;
    hal->clearBtn3();
  }

  if (hal->btn2Down()) {
    if (running) {  // Stop
      running = false;
    } else {  // Reset
      diff = 0;
      sumPaused = 0;
      reset = true;
    }
    hal->clearBtn2();
  }

  hal->getCanvas()->fillScreen(0);
  hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
  
  hal->getCanvas()->setTextSize(2);
  if (reset) {
    hal->getCanvas()->setCursor(220 - defaultFontXOffset(strlen(LANG_STW_START), 2) - 10, 42); //Note: Further offset some pixels to make text visible on round displays
    hal->getCanvas()->print(LANG_STW_START);
  } else if (!running) {
    hal->getCanvas()->setCursor(220 - defaultFontXOffset(strlen(LANG_STW_CONTINUE), 2) - 10, 42);
    hal->getCanvas()->print(LANG_STW_CONTINUE);
  }

  if (running) {
    hal->getCanvas()->setCursor(220 - defaultFontXOffset(strlen(LANG_STW_STOP), 2) - 10, 182);
    hal->getCanvas()->print(LANG_STW_STOP);
  } else if (!reset) {
    hal->getCanvas()->setCursor(220 - defaultFontXOffset(strlen(LANG_STW_RESET), 2) - 10, 182);
    hal->getCanvas()->print(LANG_STW_RESET);
  }

  if (running) {
    diff = hal->getLocalTime() - start;
  }

  long total = diff + sumPaused;
  long deltaSeconds = (total) % 60;
  long deltaMinutes = (total / 60) % 60;
  long deltaHours = (total / 60 / 60) % 24;
  long deltaDays = total / 60 / 60 / 24;

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

void OswAppStopWatch::stop(OswHal* hal) {
  // hal->disableDisplayBuffer();
}
