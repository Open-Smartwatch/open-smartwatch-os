
#include "./faces/stopwatch.h"

#include "config.h"  // Include the config here again to access the language definitions of it
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

    if (running) {
      //_launcher->getHal()->gfx()->setTextCursorBtn2();
      //_launcher->getHal()->gfx()->print(LANG_STW_STOP);
    } else if (!reset) {
      //_launcher->getHal()->gfx()->setTextCursorBtn2();
      //_launcher->getHal()->gfx()->print(LANG_STW_RESET);
    }

}
void OswStopwatchFace::action2(){

}

void OswStopwatchFace::loop() {
  OswHal* hal = _launcher->getHal();
  hal->getCanvas()->fillScreen(0);
  hal->getCanvas()->setTextColor(rgb565(255, 255, 255));

  if (reset) {
    hal->gfx()->setTextCursorBtn3();
    hal->gfx()->print(LANG_STW_START);
  } else if (!running) {
    hal->gfx()->setTextCursorBtn3();
    hal->gfx()->print(LANG_STW_CONTINUE);
  }

  if (running) {
    hal->gfx()->setTextCursorBtn2();
    hal->gfx()->print(LANG_STW_STOP);
  } else if (!reset) {
    hal->gfx()->setTextCursorBtn2();
    hal->gfx()->print(LANG_STW_RESET);
  }

  // hal->gfx()->setTextCursorBtn1();
  // hal->gfx()->print("TEST");

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

  // hal->gfx()->print(".");
  // pushing the button is inaccurate
  // also we have more space on the screen this way
  // hal->gfx()->print(deltaMillis / 100);

  _launcher->requestFlush();
}

void OswStopwatchFace::stop() {
  // hal->disableDisplayBuffer();
}
