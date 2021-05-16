
#include "./apps/main/stopwatch.h"

#include "config.h"  // Include the config here again to access the language definitions of it
#include "gfx_util.h"
#include "osw_app.h"
#include "osw_hal.h"
#include "osw_ui.h"
#include <string.h>
#include "sprites/renderer.h"

// continue after sleep does not work yet
// because millis restarts from 0
// TODO: use the I2C RTC instead
RTC_DATA_ATTR long start = 0;
RTC_DATA_ATTR long diff = 0;
RTC_DATA_ATTR bool running = false;
RTC_DATA_ATTR bool reset = true;
RTC_DATA_ATTR long sumPaused = 0;
RTC_DATA_ATTR long stepsOffset = 0;

void OswAppStopWatch::setup(OswHal* hal) {
  topText = new TextSprite(hal);
  topText->setFontSize(2);
  topText->setPosition(SCREEN_WIDTH - 30, 40);
  topText->setAlignment(SpriteAlignment::RIGHT | SpriteAlignment::CENTER_V);

  bottomText = new TextSprite(hal);
  bottomText->setFontSize(2);
  bottomText->setPosition(SCREEN_WIDTH - 30, 180);
  bottomText->setAlignment(SpriteAlignment::RIGHT | SpriteAlignment::CENTER_V);

  timeText = new TextSprite(hal);
  timeText->setFontSize(4);
  timeText->setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

  daysText = new TextSprite(hal);
  daysText->setVisible(false);
  daysText->setFontSize(4);
  daysText->setAlignment(SpriteAlignment::CENTER_H | SpriteAlignment::BOTTOM);
}

void OswAppStopWatch::loop(OswHal* hal) {
  if (hal->btnHasGoneDown(BUTTON_3)) {
    if (reset) {  // Start
      start = hal->getLocalTime();
    } else {  // Continue
      sumPaused += diff;
      start = hal->getLocalTime();
    }
    running = true;
    reset = false;
  }

  if (hal->btnHasGoneDown(BUTTON_2)) {
    if (running) {  // Stop
      running = false;
    } else {  // Reset
      diff = 0;
      sumPaused = 0;
      reset = true;
    }
  }

  topText->setVisible(reset || !running);
  if (reset) {
    topText->setText(LANG_STW_START);
  } else if (!running) {
    topText->setText(LANG_STW_CONTINUE);
  }
  
  bottomText->setVisible(running || !reset);
  if (running) {
    bottomText->setText(LANG_STW_STOP);
  } else if (!reset) {
    bottomText->setText(LANG_STW_RESET);
  }

  if (running) {
    diff = hal->getLocalTime() - start;
  }

  long total = diff + sumPaused;
  long deltaSeconds = (total) % 60;
  long deltaMinutes = (total / 60) % 60;
  long deltaHours = (total / 60 / 60) % 24;
  long deltaDays = total / 60 / 60 / 24;

  char *strBuf;
  asprintf(&strBuf, "%02ld:%02ld:%02ld", deltaHours, deltaMinutes, deltaSeconds);
  if (strBuf != NULL) {
    timeText->setText(strBuf);
    free(strBuf);
  }

  daysText->setVisible(deltaDays > 0);

  if (deltaDays) {
    asprintf(&strBuf, "%ld%c", deltaDays, 'd');

    if (strBuf != NULL) {
      daysText->setText(strBuf);
      daysText->setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - timeText->getSizeY() / 2);
      free(strBuf);
    }
  }

  Renderer::getInstance().draw(hal, ui->getBackgroundColor());
}

void OswAppStopWatch::stop(OswHal* hal) {
  //Clean up our sprites and release their memory
  Renderer::getInstance().clearAll(true);
}
