
#include "./apps/main/stopwatch.h"

#include "config_defaults.h"  // Include the config here again to access the language definitions of it
#include "gfx_util.h"
#include "osw_app.h"
#include "osw_hal.h"
#include "osw_ui.h"

// continue after sleep does not work yet
// because millis restarts from 0
// TODO: use the I2C RTC instead
RTC_DATA_ATTR long start = 0;
RTC_DATA_ATTR long diff = 0;
RTC_DATA_ATTR bool running = false;
RTC_DATA_ATTR bool reset = true;
RTC_DATA_ATTR long sumPaused = 0;
RTC_DATA_ATTR long stepsOffset = 0;

void OswAppStopWatch::setup(OswHal* hal) {}

void OswAppStopWatch::loop(OswHal* hal) {

  if (hal->btnHasGoneDown(BUTTON_3)) {
    if(running){
      running = false;
    } else {
      if (reset) {  // Start
        start = hal->getLocalTime();
      } else {  // Continue
        sumPaused += diff;
        start = hal->getLocalTime();
      }
      reset = false;
      running = true;
    }
  }

  // Update the diff before processing the stop/reset, to prevent time increased by pressing continue and reset simultaneously
  if (running) {
    diff = hal->getLocalTime() - start;
  }
  long totalTime = diff + sumPaused;

  long btnTimeout = 1800;
  long btnDown = 0;
  if(running){
    if(hal->btnHasGoneDown(BUTTON_2)){
      addLap(hal, totalTime);
    }
  } else {
    if(hal->btnIsDown(BUTTON_2)){
      btnDown = hal->btnIsDownSince(BUTTON_2);
      if(btnDown > btnTimeout){
        diff = 0;
        sumPaused = 0;
        for(char l = 0; l < maxLaps; l++){
          laps[l] = 0;
        }
        lapNum = 0;
        lapPages = 0;
        lapPage = 0;
        lastLapTime = 0;
        reset = true;
      }
    }
  }

  if(hal->btnHasGoneDown(BUTTON_1)){
    if(lapPages > 1){
      lapPage ++;
      lapPage = lapPage % lapPages;
    }
  }

  hal->gfx()->fill(ui->getBackgroundColor());
  if(lapNum > 0){
    hal->gfx()->setTextColor(ui->getPrimaryColor(), ui->getBackgroundColor());
    drawSmallTime(hal, totalTime, DISP_H * 1 / 4, 3);
    hal->gfx()->setTextColor(ui->getForegroundColor(), ui->getBackgroundColor());
  } else {
    drawTime(hal, totalTime);
  }
  drawLaps(hal);
  drawPageIndicator(hal);
  if(running){
    drawPauseButton(hal);
    drawLapButton(hal);
  } else {
    drawStartButton(hal);
    if(!reset){
      drawResetButton(hal, btnDown * 270 / btnTimeout);
    }
  }
  hal->requestFlush();
}

void OswAppStopWatch::addLap(OswHal* hal, long totalTime) {
  if(lapNum < maxLaps){
    laps[lapNum] = totalTime - lastLapTime;
    lapNum ++;
    lapPages = (lapNum-1) / lapsPerPage + 1;
    lastLapTime = totalTime;
  }
}

void OswAppStopWatch::stop(OswHal* hal) {}


void OswAppStopWatch::drawTime(OswHal* hal, long totalTime) {
  long deltaSeconds = (totalTime) % 60;
  long deltaMinutes = (totalTime / 60) % 60;
  long deltaHours = (totalTime / 60 / 60) % 24;


  long deltaDays = totalTime / 60 / 60 / 24;

  hal->getCanvas()->setTextSize(4);

  if (deltaDays) {
    hal->gfx()->setTextSize(4);
    hal->gfx()->setTextBottomAligned();
    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextCursor(DISP_W / 2, DISP_H / 2);
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
  hal->gfx()->setTextCursor(DISP_W / 2 - hal->gfx()->getTextOfsetColumns(4), DISP_H / 2);
  hal->gfx()->printDecimal(deltaHours, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(deltaMinutes, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(deltaSeconds, 2);

  // hal->gfx()->print(".");
  // pushing the button is inaccurate
  // also we have more space on the screen this way
  // hal->gfx()->print(deltaMillis / 100);
}

void OswAppStopWatch::drawSmallTime(OswHal* hal, long totalTime, long y, char size) {
  long deltaSeconds = (totalTime) % 60;
  long deltaMinutes = (totalTime / 60) % 60;
  long deltaHours = (totalTime / 60 / 60) % 24;

  hal->gfx()->setTextSize(size);
  hal->gfx()->setTextMiddleAligned();
  hal->gfx()->setTextCursor(DISP_W * 5 / 12 - hal->gfx()->getTextOfsetColumns(4), y);
  hal->gfx()->printDecimal(deltaHours, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(deltaMinutes, 2);
  hal->gfx()->print(":");
  hal->gfx()->printDecimal(deltaSeconds, 2);
}

void OswAppStopWatch::drawLaps(OswHal* hal) {
  char moved = lapsPerPage * lapPage;
  for(char i = 0; i < lapsPerPage && i + moved < lapNum; i++){
    drawSmallTime(hal, laps[lapNum - 1 - moved - i], DISP_H * 1 / 4 + (i * 20 + 30), 2);
  }
  /*
  for(char i = 0; i < lapNum; i++){
    drawSmallTime(hal, laps[i], DISP_H * 1 / 4 + ((lapNum - i) * 20 + 10), 2);
  }
  */
}

void OswAppStopWatch::drawStartButton(OswHal* hal) {
  hal->gfx()->drawThickLine(DISP_W * 9 / 12, DISP_H * 3 / 12, DISP_W * 9 / 12, DISP_H * 4 / 12, DISP_H * 1/4 / 12, ui->getForegroundColor());
  hal->gfx()->drawThickLine(DISP_W * 9 / 12, DISP_H * 3 / 12, DISP_W * 39/4 / 12, DISP_H * 7/2 / 12, DISP_H * 1/4 / 12, ui->getForegroundColor());
  hal->gfx()->drawThickLine(DISP_W * 9 / 12, DISP_H * 4 / 12, DISP_W * 39/4 / 12, DISP_H * 7/2 / 12, DISP_H * 1/4 / 12, ui->getForegroundColor());
}

void OswAppStopWatch::drawPauseButton(OswHal* hal) {
  hal->gfx()->drawThickLine(DISP_W * 9 / 12, DISP_H * 3 / 12, DISP_W * 9 / 12, DISP_H * 4 / 12, DISP_H * 1/4 / 12, ui->getForegroundColor());
  hal->gfx()->drawThickLine(DISP_W * 10 / 12, DISP_H * 3 / 12, DISP_W * 10 / 12, DISP_H * 4 / 12, DISP_H * 1/4 / 12, ui->getForegroundColor());
}

void OswAppStopWatch::drawLapButton(OswHal* hal) {
  if(lapNum < maxLaps){
    hal->gfx()->drawThickLine(DISP_W * 9 / 12, DISP_H * 16/2 / 12, DISP_W * 9 / 12, DISP_H * 19/2 / 12, DISP_H * 1/8 / 12, lapNum < maxLaps - lapsPerPage ? ui->getForegroundColor() : ui->getWarningColor());
    hal->gfx()->drawThickLine(DISP_W * 9 / 12, DISP_H * 16/2 / 12, DISP_W * 10 / 12, DISP_H * 17/2 / 12, DISP_H * 1/8 / 12, lapNum < maxLaps - lapsPerPage ? ui->getForegroundColor() : ui->getWarningColor());
    hal->gfx()->drawThickLine(DISP_W * 9 / 12, DISP_H * 18/2 / 12, DISP_W * 10 / 12, DISP_H * 17/2 / 12, DISP_H * 1/8 / 12, lapNum < maxLaps - lapsPerPage ? ui->getForegroundColor() : ui->getWarningColor());
  }
}

void OswAppStopWatch::drawResetButton(OswHal* hal, long btn) {
  hal->gfx()->drawArc(DISP_W * 19/2 / 12, DISP_H * 17/2 / 12, 0, 270, 90, DISP_W * 3/4 / 12, 3, ui->getForegroundColor());
  if(btn > 0){
    hal->gfx()->drawArc(DISP_W * 19/2 / 12, DISP_H * 17/2 / 12, 270-btn, 270, 90, DISP_W * 3/4 / 12, 3, ui->getWarningColor());
  }
  hal->gfx()->drawThickLine(DISP_W * 34/4 / 12, DISP_H * 17/2 / 12, DISP_W * 36/4 / 12, DISP_H * 17/2 / 12, 3, btn > 0 ? ui->getWarningColor() : ui->getForegroundColor());
  hal->gfx()->drawThickLine(DISP_W * 34/4 / 12, DISP_H * 17/2 / 12, DISP_W * 35/4 / 12, DISP_H * 16/2 / 12, 3, btn > 0 ? ui->getWarningColor() : ui->getForegroundColor());
  hal->gfx()->drawThickLine(DISP_W * 36/4 / 12, DISP_H * 17/2 / 12, DISP_W * 35/4 / 12, DISP_H * 16/2 / 12, 3, btn > 0 ? ui->getWarningColor() : ui->getForegroundColor());
}

void OswAppStopWatch::drawPageIndicator(OswHal* hal) {
  if(lapPages > 1){
    uint16_t spacing = 8; // Distance (deg) between dots
    uint16_t x = 8;
    uint16_t y0 = DISP_H / 2 - (spacing / 2 * (lapPages-1));
    uint16_t rDot = 4; // Size (radius) of the dot
    uint16_t r = (DISP_W / 2) - 8; // Distance from the center of the watch (radius)
    uint16_t alpha0 = 147 + (spacing / 2 * (lapPages-1)); // Angle of the first Element (147deg = Button 1)
    for(uint8_t i = 0; i < lapPages ; i++){
      uint16_t alpha = alpha0 - (i * spacing);
      uint16_t x = (DISP_W / 2) + (cos(alpha * PI / 180) * r);
      uint16_t y = (DISP_H / 2) + (sin(alpha * PI / 180) * r);
      if(i == lapPage){
        //hal->gfx()->fillCircle(x, y0 + (spacing * i), r, ui->getInfoColor());
        hal->gfx()->fillCircle(x, y, rDot, ui->getInfoColor());
      } else {
        //hal->gfx()->fillCircle(x, y0 + (spacing * i), r, ui->getForegroundColor());
        hal->gfx()->fillCircle(x, y, rDot, ui->getForegroundColor());
      }
    }
  }
}