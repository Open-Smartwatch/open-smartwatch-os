#include "./apps/clock/stopwatch.h"

#include "config_defaults.h"  // Include the config here again to access the language definitions of it
#include "gfx_util.h"
#include <OswAppV1.h>
#include "osw_hal.h"
#include "osw_ui.h"

// continue after sleep does not work yet
// because millis restarts from 0
// todo: use the I2C RTC instead
// ↑ @author, are you sure? We added an automatic resync to the ESPs internal RTC, so it should work now...
RTC_DATA_ATTR long start = 0;
RTC_DATA_ATTR long diff = 0;
RTC_DATA_ATTR bool running = false;
RTC_DATA_ATTR bool reset = true;
RTC_DATA_ATTR long sumPaused = 0;
RTC_DATA_ATTR long stepsOffset = 0;
RTC_DATA_ATTR bool overallTime = true;

void OswAppStopWatch::setup() {}

void OswAppStopWatch::loop() {
    OswHal* hal = OswHal::getInstance();
    if (hal->btnHasGoneDown(BUTTON_3)) {
        if(running) {
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

    long btnTimeout = TOOL_STOPWATCH_BTN_TIMEOUT;
    long btnDown = 0;
    if(running) {
        if(hal->btnHasGoneDown(BUTTON_2)) {
            addLap(totalTime);
        }
    } else {
        if (hal->btnHasGoneDown(BUTTON_2)) {
            overallTime = !overallTime;
        }
        if(hal->btnIsDown(BUTTON_2)) { // Reset
            btnDown = hal->btnIsDownFor(BUTTON_2);
            if(btnDown > btnTimeout) {
                diff = 0;
                sumPaused = 0;
                for(unsigned char l = 0; l < maxLaps; l++) {
                    laps[l] = 0;
                    overall[l] = 0;
                }
                lapNum = 0;
                lapPages = 0;
                lapPage = 0;
                lastLapTime = 0;
                reset = true;
            }
        }
    }

    if(hal->btnHasGoneDown(BUTTON_1)) {
        if(lapPages > 1) {
            lapPage ++;
            lapPage = lapPage % lapPages;
        }
    }

    if(lapNum > 0) { // First laps-time on the little bar
        drawTime(totalTime, DISP_H/4, 3);
        hal->gfx()->drawThickLine(DISP_W*1/4, DISP_H/2, DISP_W*3/4, DISP_H/2, 2, ui->getPrimaryColor());
        hal->gfx()->drawThickLine(DISP_W*10/12, DISP_H/2, DISP_W*11/12, DISP_H/2, 2, ui->getPrimaryColor());
    } else {
        drawTime(totalTime, DISP_H/2, 4); // not laps-time (Big font)
    }
    drawLaps();
    drawPageIndicator();
    if(running) {
        drawPauseButton();
        drawLapButton();
    } else {
        drawStartButton();
        if(!reset) {
            drawResetButton(btnDown * 270 / btnTimeout);
        }
    }
}

void OswAppStopWatch::addLap(long totalTime) {
    if(lapNum < maxLaps) {
        overall[lapNum] = totalTime;
        laps[lapNum] = totalTime - lastLapTime;
        lapNum ++;
        lapPages = (lapNum-2) / lapsPerPage + 1;
        lastLapTime = totalTime;
    }
}

void OswAppStopWatch::stop() {}

void OswAppStopWatch::drawTime(long totalTime, long y, char size) {
    long deltaSeconds = (totalTime) % 60;
    long deltaMinutes = (totalTime / 60) % 60;
    long deltaHours = (totalTime / 60 / 60) % 24;

    OswHal* hal = OswHal::getInstance();
    hal->gfx()->setTextSize(size);
    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextCursor(DISP_W / 2 - hal->gfx()->getTextOfsetColumns(4), y);
    hal->gfx()->printDecimal(deltaHours, 2);
    hal->gfx()->print(":");
    hal->gfx()->printDecimal(deltaMinutes, 2);
    hal->gfx()->print(":");
    hal->gfx()->printDecimal(deltaSeconds, 2);
}

void OswAppStopWatch::drawLaps() {
    if(lapNum > 0) {
        if(overallTime)
            drawTime(overall[lapNum - 1], (DISP_H / 4) + 40, 2);
        else
            drawTime(laps[lapNum - 1], (DISP_H / 4) + 40, 2);
    }
    char moved = lapsPerPage * lapPage;
    for(char i = 0; i < lapsPerPage && (i+1) + moved < lapNum; i++) {
        long y = (DISP_H / 4) + ((i+1) * 20) + 60;
        if (overallTime)
            drawTime(overall[lapNum - 1 - moved - (i + 1)], y, 2);
        else
            drawTime(laps[lapNum - 1 - moved - (i + 1)], y, 2);
    }
}

void OswAppStopWatch::drawStartButton() {
    uint16_t color = ui->getForegroundColor();
    int top = DISP_H / 4;
    int left = DISP_W * 3 / 4;
    int height = DISP_H / 12;
    int width = DISP_W  / 16;
    int radius = DISP_H / 48;
    if(lapNum > 0) {
        height /= 2;
        width /= 2;
        radius /=2;
        top += height * 7/2;
        left = (DISP_W * 21/24) - (width/2);
    }
    OswHal* hal = OswHal::getInstance();
    hal->gfx()->drawThickLine(left, top, left, top + height, radius, color);
    hal->gfx()->drawThickLine(left, top, left + width, top + (height/2), radius, color);
    hal->gfx()->drawThickLine(left, top + height, left + width, top + (height/2), radius, color);
}

void OswAppStopWatch::drawPauseButton() {
    uint16_t color = ui->getForegroundColor();
    int top = DISP_H * 3 / 12;
    int left = DISP_W * 3 / 4;
    int height = DISP_H / 12;
    int width = DISP_W / 12;
    int radius = DISP_H / 48;
    if(lapNum > 0) {
        height /= 2;
        width /= 2;
        radius /= 2;
        top += height * 7/2;
        left = (DISP_W * 21/24) - (width/2);
    }
    OswHal* hal = OswHal::getInstance();
    hal->gfx()->drawThickLine(left, top, left, top + height, radius, color);
    hal->gfx()->drawThickLine(left + width, top, left + width, top + height, radius, color);
}

void OswAppStopWatch::drawLapButton() {
    uint16_t color = ui->getForegroundColor();
    int top = DISP_H * 2 / 3;
    int left = DISP_W * 3 / 4;
    int height = DISP_H * 3 / 24;
    int width = DISP_W * 5 / 48;
    int radius = DISP_H / 96;
    if(lapNum > 0) {
        height /= 2;
        width /= 2;
        radius /= 2;
        top -= height * 7/4;
        left = (DISP_W * 21/24) - (width/2);
    }
    if(lapNum < maxLaps) {
        OswHal* hal = OswHal::getInstance();
        hal->gfx()->drawThickLine(left, top, left, top + height, radius, color);
        hal->gfx()->fillFrame(left, top, width - (height/4), height/2, color);
        hal->gfx()->fillCircle(left + width - (height/4), top + (height/4), height/4, color);
    }
}

void OswAppStopWatch::drawResetButton(long btn) {
    OswHal* hal = OswHal::getInstance();
    uint16_t color = ui->getForegroundColor();
    int top = DISP_H * 31/4 / 12;
    int left = DISP_W * 35/4 / 12;
    int height = DISP_W * 6/4 / 12;
    int width = DISP_W * 6/4 / 12;
    int radius = 3;

    if(lapNum > 0) {
        height /= 2;
        width /= 2;
        radius /= 2;
        top -= height * 7/4;
        left = (DISP_W * 21/24) - (width/2);
    }
    hal->gfx()->drawArc(left + (width/2), top + (height/2), 0, 270, 90, width/2, radius, color);
    if(btn > 0) {
        hal->gfx()->drawArc(left + (width/2), top + (height/2), 270-btn, 270, 90, width/2, radius, ui->getPrimaryColor());
    }
    hal->gfx()->drawThickLine(left - (width/6), top + (height/2), left + (width/6), top + (height/2), radius, color);
    hal->gfx()->drawThickLine(left - (width/6), top + (height/2), left, top + (height/4), radius, color);
    hal->gfx()->drawThickLine(left + (width/6), top + (height/2), left, top + (height/4), radius, color);
}

void OswAppStopWatch::drawPageIndicator() {
    if(lapPages > 1) {
        uint16_t spacing = 8; // Distance (deg) between dots
        uint16_t rDot = 4; // Size (radius) of the dot
        uint16_t r = (DISP_W / 2) - 8; // Distance from the center of the watch (radius)
        uint16_t alpha0 = 147 + (spacing / 2 * (lapPages-1)); // Angle of the first Element (147deg = Button 1)
        for(uint8_t i = 0; i < lapPages ; i++) {
            uint16_t alpha = alpha0 - (i * spacing);
            uint16_t x = (DISP_W / 2) + (cosf(alpha * PI / 180) * r);
            uint16_t y = (DISP_H / 2) + (sinf(alpha * PI / 180) * r);
            if(i == lapPage) {
                OswHal::getInstance()->gfx()->fillCircle(x, y, rDot, ui->getPrimaryColor());
            } else {
                OswHal::getInstance()->gfx()->fillCircle(x, y, rDot, ui->getForegroundColor());
            }
        }
    }
}