
#include "./faces/stopwatch.h"

#include "config.h"
#include "gfx_util.h"
#include "osw_app.h"
#include "osw_hal.h"
#include "osw_ui_util.h"

// continue after sleep does not work yet
// because millis restarts from 0
// TODO: use the I2C RTC instead
RTC_DATA_ATTR long start = 0;
RTC_DATA_ATTR long diff = 0;
RTC_DATA_ATTR bool running = false;
RTC_DATA_ATTR bool reset = true;
RTC_DATA_ATTR long sumPaused = 0;

// OswAppHelloWorld::OswAppHelloWorld(void) : OswApp() {}
void OswStopwatchFace::setup(OswLauncher*launcher ) {
  _launcher = launcher;
}

void OswStopwatchFace::action(){
     if (reset) {  // Start
      start = _launcher->getHal()->getLocalTime();
    } else {  // Continue
      sumPaused += diff;
      start = _launcher->getHal()->getLocalTime();
    }
    running = true;
    reset = false;
    _launcher->getHal()->clearBtn3();
}
void OswStopwatchFace::action2(){
 if (running) {  // Stop
      running = false;
    } else {  // Reset
      diff = 0;
      sumPaused = 0;
      reset = true;
    }
    _launcher->getHal()->clearBtn2();
}

void OswStopwatchFace::loop() {
  OswHal* hal = _launcher->getHal();
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
    diff = hal->getLocalTime() - start;
  }

  long total = diff + sumPaused;
  long deltaSeconds = (total) % 60;
  long deltaMinutes = (total / 60) % 60;
  long deltaHours = (total / 60 / 60) % 24;
  long deltaDays = total / 60 / 60 / 24;

    if (deltaDays) {
      hal->gfx()->setTextSize(4);
      hal->gfx()->setTextBottomAligned();
      hal->gfx()->setTextCenterAligned();
      hal->gfx()->setTextCursor(120, 120);
      hal->gfx()->print((String(deltaDays) + "d").c_str());
    }

    hal->gfx()->setTextSize(4);
    if (deltaDays) {
      hal->gfx()->setTextTopAligned();
    } else {
      hal->gfx()->setTextMiddleAligned();
    }
    hal->gfx()->setTextLeftAligned();
    // manually center the counter:
    hal->gfx()->setTextCursor(120 - hal->gfx()->getTextOfsetColumns(4), 120);
    hal->gfx()->printDecimal(deltaHours, 2);
    hal->gfx()->print(":");
    hal->gfx()->printDecimal(deltaMinutes, 2);
    hal->gfx()->print(":");
    hal->gfx()->printDecimal(deltaSeconds, 2);

  _launcher->requestFlush();
}

void OswStopwatchFace::stop() {
  // hal->disableDisplayBuffer();
}
