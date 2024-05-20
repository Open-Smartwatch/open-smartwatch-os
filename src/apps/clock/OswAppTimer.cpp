#include "./apps/clock/OswAppTimer.h"

void OswAppTimer::setup() {}

void OswAppTimer::stop() {}

OswAppTimer::OswAppTimer() {
    state = TimerState::IDLE;
}

void OswAppTimer::handleNextButton(const unsigned char optionsCount) {
    if (OswHal::getInstance()->btnHasGoneDown(BUTTON_1)) {
        step = (step + 1) % optionsCount;
    }
}

void OswAppTimer::resetTimer() {
    state = TimerState::IDLE;
    step = {};
    timestamp = {};
    timerLeftSec = {};
}

void OswAppTimer::handleIncrementButton() {
    if (OswHal::getInstance()->btnHasGoneDown(BUTTON_3)) {
        switch (step) {
        case 0:
        case 1:
        case 3:
        case 5:
            timestamp[step] = (timestamp[step] + 1) % 10;
            break;
        case 2:
        case 4:
            timestamp[step] = (timestamp[step] + 1) % 6;
            break;
        case 6:
            resetTimer();
            break;
        case 7: {
            state = TimerState::RUNNING;
            auto utcTime = std::chrono::system_clock::from_time_t(OswHal::getInstance()->getUTCTime());
            auto currentTime = utcTime + std::chrono::seconds{static_cast<int>(OswHal::getInstance()->getTimezoneOffsetPrimary())};
            timeToFire = std::chrono::time_point_cast<std::chrono::seconds>(currentTime) + timerLeftSec;
            notificationId = notifierClient.createNotification(timeToFire).second.getId();
        }
        break;
        }
    }
}

void OswAppTimer::handleDecrementButton() {
    if (OswHal::getInstance()->btnHasGoneDown(BUTTON_2)) {
        switch (step) {
        case 0:
        case 1:
        case 3:
        case 5:
            timestamp[step] = (timestamp[step] + 9) % 10;
            break;
        case 2:
        case 4:
            timestamp[step] = (timestamp[step] + 5) % 6;
            break;
        }
    }
}

long OswAppTimer::handleResetButton() {
    auto* hal = OswHal::getInstance();
    const auto btnTimeout = TOOL_TIMER_BTN_TIMEOUT;
    long btnDown = 0;
    if (hal->btnIsDown(BUTTON_2)) {
        // Reset
        btnDown = hal->btnIsDownFor(BUTTON_2);
        if (btnDown > btnTimeout) {
            state = TimerState::IDLE;
            notifierClient.deleteNotification(notificationId);
            resetTimer();
        }
    }
    return btnDown;
}

void OswAppTimer::timestampToSec() {
    const auto hours = timestamp[0] * 10 + timestamp[1];
    const auto minutes = timestamp[2] * 10 + timestamp[3];
    const auto seconds = timestamp[4] * 10 + timestamp[5];
    timerLeftSec = std::chrono::seconds{hours * 3600 + minutes * 60 + seconds};
}

void OswAppTimer::drawNumber(const int number, const int index) {
    auto* hal = OswHal::getInstance();
    auto* ui = OswUI::getInstance();
    const auto colorActive = ui->getDangerColor();
    const auto colorForeground = ui->getForegroundColor();
    const auto colorBackground = ui->getBackgroundColor();
    hal->gfx()->setTextColor(state == TimerState::SET_TIMER_SCREEN && index == step ? colorActive : colorForeground, colorBackground);
    hal->gfx()->print(number);
    ui->resetTextColors();
}

void OswAppTimer::drawTime(const int totalSeconds) {
    auto* hal = OswHal::getInstance();

    hal->gfx()->resetText();

    hal->gfx()->setTextSize(4);
    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextCursor(DISP_W / 2 - hal->gfx()->getTextOfsetColumns(4), DISP_H / 2);

    const auto hours = totalSeconds / 3600;
    drawNumber(hours / 10, 0); // [0]0:00:00
    drawNumber(hours % 10, 1); // 0[0]:00:00
    hal->gfx()->print(":");
    const auto minutes = (totalSeconds / 60) % 60;
    drawNumber(minutes / 10, 2); // 00:[0]0:00
    drawNumber(minutes % 10, 3); // 00:0[0]:00
    hal->gfx()->print(":");
    const auto seconds = totalSeconds % 60;
    drawNumber(seconds / 10, 4); // 00:00:[0]0
    drawNumber(seconds % 10, 5); // 00:00:0[0]
}

void drawTimerIcon(uint16_t color) {
    auto* hal = OswHal::getInstance();
    auto* ui = OswUI::getInstance();

    ui->setTextCursor(BUTTON_3);
    const auto centerX = hal->gfx()->getTextCursorX() - 5;
    const auto centerY = hal->gfx()->getTextCursorY() + 10;
    const int radius = 10;

    // Clock face
    hal->gfx()->drawCircle(centerX, centerY, radius, color);

    // Clock arc
    const auto colorGreen = ui->getSuccessColor();
    hal->gfx()->drawCircle(centerX, centerY, radius - 4, colorGreen, CIRC_OPT::DRAW_UPPER_LEFT);
    hal->gfx()->drawCircle(centerX, centerY, radius - 4, colorGreen, CIRC_OPT::DRAW_LOWER_LEFT);
    hal->gfx()->drawCircle(centerX, centerY, radius - 4, colorGreen, CIRC_OPT::DRAW_LOWER_RIGHT);

    hal->gfx()->fillCircle(centerX, centerY, radius - 4, colorGreen, CIRC_OPT::DRAW_UPPER_LEFT);
    hal->gfx()->fillCircle(centerX, centerY, radius - 4, colorGreen, CIRC_OPT::DRAW_LOWER_LEFT);
    hal->gfx()->fillCircle(centerX, centerY, radius - 4, colorGreen, CIRC_OPT::DRAW_LOWER_RIGHT);

    // Clock "buttons"
    hal->gfx()->drawThickLine(centerX - radius / 4, centerY - radius - 3, centerX + radius / 4, centerY - radius - 3, 1, color);
    hal->gfx()->drawThickLine(centerX + radius - 2, centerY - radius, centerX + radius, centerY - radius + 2, 1, color, true);
}

void drawSetTimerScreen(unsigned char step, uint16_t colorActive, uint16_t colorForeground, uint16_t colorBackground) {
    auto* hal = OswHal::getInstance();
    auto* ui = OswUI::getInstance();

    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextCenterAligned();
    ui->setTextCursor(BUTTON_1);
    hal->gfx()->print(">");

    ui->setTextCursor(BUTTON_2);
    hal->gfx()->print("-");

    ui->setTextCursor(BUTTON_3);
    hal->gfx()->print("+");

    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextRightAligned();
    hal->gfx()->setTextCursor((DISP_W / 2) - hal->gfx()->getTextOfsetColumns(0.5f), DISP_H * 5 / 8);
    hal->gfx()->setTextColor(step == 6 ? colorActive : colorForeground, colorBackground);
    hal->gfx()->print(LANG_CANCEL);

    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextCursor((DISP_W / 2) + hal->gfx()->getTextOfsetColumns(0.5f), DISP_H * 5 / 8);
    hal->gfx()->setTextColor(step == 7 ? colorActive : colorForeground, colorBackground);
    hal->gfx()->print(LANG_START);
}

void drawStartButton() {
    auto* ui = OswUI::getInstance();

    uint16_t color = ui->getForegroundColor();
    int top = DISP_H / 6;
    int left = DISP_W * 3 / 4;
    int height = DISP_H / 12;
    int width = DISP_W / 16;
    int radius = DISP_H / 48;

    OswHal* hal = OswHal::getInstance();
    hal->gfx()->drawThickLine(left, top, left, top + height, radius, color);
    hal->gfx()->drawThickLine(left, top, left + width, top + (height / 2), radius, color);
    hal->gfx()->drawThickLine(left, top + height, left + width, top + (height / 2), radius, color);
}

void drawPauseButton() {
    auto* ui = OswUI::getInstance();

    uint16_t color = ui->getForegroundColor();
    int top = DISP_H / 6;
    int left = DISP_W * 3 / 4;
    int height = DISP_H / 12;
    int width = DISP_W / 12;
    int radius = DISP_H / 48;

    OswHal* hal = OswHal::getInstance();
    hal->gfx()->drawThickLine(left, top, left, top + height, radius, color);
    hal->gfx()->drawThickLine(left + width, top, left + width, top + height, radius, color);
}

void drawResetButton(long btn) {
    OswHal* hal = OswHal::getInstance();
    auto* ui = OswUI::getInstance();

    uint16_t color = ui->getForegroundColor();
    int top = (DISP_H * 31/4 / 12) + 20;
    int left = DISP_W * 35/4 / 12;
    int height = DISP_W * 6/4 / 12;
    int width = DISP_W * 6/4 / 12;
    int radius = 3;

    hal->gfx()->drawArc(left + (width/2), top + (height/2), 0, 270, 90, width/2, radius, color);
    if(btn > 0) {
        hal->gfx()->drawArc(left + (width/2), top + (height/2), 270-btn, 270, 90, width/2, radius, ui->getPrimaryColor());
    }
    hal->gfx()->drawThickLine(left - (width/6), top + (height/2), left + (width/6), top + (height/2), radius, color);
    hal->gfx()->drawThickLine(left - (width/6), top + (height/2), left, top + (height/4), radius, color);
    hal->gfx()->drawThickLine(left + (width/6), top + (height/2), left, top + (height/4), radius, color);
}

void drawRunningTimerScreen(long btnDown) {
    drawPauseButton();
    drawResetButton(btnDown * 270 / TOOL_TIMER_BTN_TIMEOUT);
}

void drawPausedTimerScreen(long btnDown) {
    drawStartButton();
    drawResetButton(btnDown * 270 / TOOL_TIMER_BTN_TIMEOUT);
}

void OswAppTimer::loop() {
    auto* hal = OswHal::getInstance();
    auto* ui = OswUI::getInstance();
    const auto colorActive = ui->getDangerColor();
    const auto colorForeground = ui->getForegroundColor();
    const auto colorBackground = ui->getBackgroundColor();

    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextCursor((DISP_W / 2), DISP_H * 2 / 12);
    hal->gfx()->print(LANG_TIMER);

    long btnDown = 0;

    switch (state) {
    case TimerState::IDLE: {
        if (hal->btnHasGoneDown(BUTTON_3)) {
            state = TimerState::SET_TIMER_SCREEN;
        }
    }
    break;

    case TimerState::SET_TIMER_SCREEN: {
        handleNextButton(8);
        handleIncrementButton();
        handleDecrementButton();

        timestampToSec();
    }
    break;

    case TimerState::RUNNING: {
        // Timer is running (countdown)
        auto utcTime = std::chrono::system_clock::from_time_t(OswHal::getInstance()->getUTCTime());
        auto currentTime = utcTime + std::chrono::seconds{static_cast<int>(OswHal::getInstance()->getTimezoneOffsetPrimary())};
        timerLeftSec = std::chrono::duration_cast<std::chrono::seconds>(timeToFire - currentTime);

        if (timerLeftSec.count() <= 0) {
            // Timer ends
            resetTimer();
        }

        btnDown = handleResetButton();

        if (hal->btnHasGoneDown(BUTTON_3)) {
            state = TimerState::PAUSED;
            notifierClient.deleteNotification(notificationId);
            timerPauseTime = std::chrono::time_point_cast<std::chrono::seconds>(currentTime);
        }
    }
    break;

    case TimerState::PAUSED: {
        timerLeftSec = timeToFire - timerPauseTime;

        btnDown = handleResetButton();

        if (hal->btnHasGoneDown(BUTTON_3)) {
            state = TimerState::RUNNING;
            auto utcTime = std::chrono::system_clock::from_time_t(OswHal::getInstance()->getUTCTime());
            auto currentTime = utcTime + std::chrono::seconds{static_cast<int>(OswHal::getInstance()->getTimezoneOffsetPrimary())};
            timeToFire = std::chrono::time_point_cast<std::chrono::seconds>(currentTime) + timerLeftSec;
            notificationId = notifierClient.createNotification(timeToFire).second.getId();
        }
    }
    break;
    }

    // Draw timer (Note: drawing must be after calculations to draw updated time)
    drawTime(timerLeftSec.count());
    switch (state) {
    case TimerState::IDLE:
        drawTimerIcon(colorForeground);
        break;
    case TimerState::SET_TIMER_SCREEN:
        drawSetTimerScreen(step, colorActive, colorForeground, colorBackground);
        break;
    case TimerState::RUNNING:
        drawRunningTimerScreen(btnDown);
        break;
    case TimerState::PAUSED:
        drawPausedTimerScreen(btnDown);
        break;
    }
}