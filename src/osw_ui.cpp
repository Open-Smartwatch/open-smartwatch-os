#include <cassert>

#include <overlays/overlays.h>
#include <osw_config.h>
#include <OswAppV2.h>

#include <osw_ui.h>

std::unique_ptr<OswUI> OswUI::instance = nullptr;
OswUI::OswUI() {
    this->drawLock.reset(new std::mutex());
};

OswUI* OswUI::getInstance() {
    if (OswUI::instance == nullptr)
        OswUI::instance.reset(new OswUI());
    return OswUI::instance.get();
};

void OswUI::resetInstance() {
    return OswUI::instance.reset();
};

uint16_t OswUI::getBackgroundColor(void) {
    return rgb888to565(OswConfigAllKeys::themeBackgroundColor.get());
}
uint16_t OswUI::getBackgroundDimmedColor(void) {
    return rgb888to565(OswConfigAllKeys::themeBackgroundDimmedColor.get());
}
uint16_t OswUI::getForegroundColor(void) {
    return rgb888to565(OswConfigAllKeys::themeForegroundColor.get());
}
uint16_t OswUI::getForegroundDimmedColor(void) {
    return rgb888to565(OswConfigAllKeys::themeForegroundDimmedColor.get());
}
uint16_t OswUI::getPrimaryColor(void) {
    return rgb888to565(OswConfigAllKeys::themePrimaryColor.get());
}
uint16_t OswUI::getInfoColor(void) {
    return rgb888to565(OswConfigAllKeys::themeInfoColor.get());
}
uint16_t OswUI::getSuccessColor(void) {
    return rgb888to565(OswConfigAllKeys::themeSuccessColor.get());
}
uint16_t OswUI::getWarningColor(void) {
    return rgb888to565(OswConfigAllKeys::themeWarningColor.get());
}
uint16_t OswUI::getDangerColor(void) {
    return rgb888to565(OswConfigAllKeys::themeDangerColor.get());
}

void OswUI::resetTextColors(void) {
    Graphics2DPrint* gfx = OswHal::getInstance()->gfx();
    assert(gfx != nullptr);
    gfx->setTextColor(rgb888to565(OswConfigAllKeys::themeForegroundColor.get()),
                      rgb888to565(OswConfigAllKeys::themeBackgroundColor.get()));
}

void OswUI::resetTextAlignment() {
    OswHal* hal = OswHal::getInstance();
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextBottomAligned();
}

void OswUI::setTextCursor(Button btn) {
    // TODO this should not also modify the text size, right?
    OswHal* hal = OswHal::getInstance();
    hal->gfx()->setTextSize(2);
    int16_t x, y;
    hal->getButtonCoordinates(btn, x, y);
    if(hal->btnIsLeftAligned(btn))
        hal->gfx()->setTextLeftAligned();
    else
        hal->gfx()->setTextRightAligned();
    if(hal->btnIsTopAligned(btn))
        hal->gfx()->setTextTopAligned();
    else
        hal->gfx()->setTextBottomAligned();
    hal->gfx()->setTextCursor(x, y);
}

void OswUI::setRootApplication(OswAppV2* rootApplication) {
    if(this->mRootApplication != nullptr)
        this->mRootApplication->onStop();
    this->mRootApplication = rootApplication;
    if(this->mRootApplication != nullptr)
        this->mRootApplication->onStart();
}

OswAppV2* OswUI::getRootApplication() {
    return this->mRootApplication;
}

void OswUI::loop() {
    {
        std::lock_guard<std::mutex> notifyGuard(this->mNotificationsLock);
        auto notificationsDismissed = !this->mNotifications.empty() && (OswHal::getInstance()->btnHasGoneDown(BUTTON_1) ||
                                      OswHal::getInstance()->btnHasGoneDown(BUTTON_2) ||
                                      OswHal::getInstance()->btnHasGoneDown(BUTTON_3));
        // Drop all timed out notifications
        for (auto it = this->mNotifications.begin(); it != this->mNotifications.end();) {
            if (it->getEndTime() <= millis() || notificationsDismissed) {
                it = this->mNotifications.erase(it);
            } else {
                ++it;
            }
        }
        if (notificationsDismissed) {
            // Don't propagate the OswHall::btnHasGoneDown() event further
            return;
        }
    }

    OswAppV2* rootApp = this->mRootApplication; // Create local copy of current root app, as it may change during the loop() call
    if(rootApp == nullptr) {
        OSW_LOG_E("No root application set!");
        return; // Early abort if no app is set
    }
    rootApp->onLoop();
#ifdef OSW_EMULATOR
#ifndef NDEBUG
    if(!OswEmulator::instance->isHeadless)
        rootApp->onLoopDebug();
#endif
#endif

    // Lock UI for drawing
    if(rootApp->getNeedsRedraw() or (rootApp->getViewFlags() & OswAppV2::ViewFlags::NO_FPS_LIMIT)) {
        if(not (rootApp->getViewFlags() & OswAppV2::ViewFlags::NO_FPS_LIMIT) and this->mEnableTargetFPS and (millis() - lastFlush) < (1000 / this->mTargetFPS))
            return; // Early abort if we would draw too fast
        std::lock_guard<std::mutex> guard(*this->drawLock); // Make sure to not modify the notifications vector during drawing

        // BG
        if (OswHal::getInstance()->displayBufferEnabled())
            OswHal::getInstance()->gfx()->fill(this->getBackgroundColor());
        else if (this->lastBGFlush < millis() - 10000) {
            // In case the buffering is inactive, only flush every 10 seconds the whole buffer
            OswHal::getInstance()->gfx()->fill(this->getBackgroundColor());
            this->lastBGFlush = millis();
        }

        this->resetTextColors();
        this->resetTextAlignment();
        if (this->mProgressBar == nullptr) {
            // Apps
            OswHal::getInstance()->gfx()->setTextLeftAligned();
            OswHal::getInstance()->gfx()->setTextSize(1.0f);
            rootApp->onDraw();
            rootApp->onDrawOverlay();
        } else {
            // Full-Screen progress
            OswHal::getInstance()->gfx()->setTextCenterAligned();
            OswHal::getInstance()->gfx()->setTextSize(2.0f);
            OswHal::getInstance()->gfx()->setTextCursor(DISP_W * 0.5f, DISP_H * 0.5f);
            OswHal::getInstance()->gfx()->print(this->mProgressText);
            this->mProgressBar->draw();
        }

        this->resetTextColors();
        {
            std::lock_guard<std::mutex> notifyGuard(this->mNotificationsLock);
            // Draw all notifications
            auto y = DISP_H - OswUINotification::sDrawHeight;
            for (const auto& notification : this->mNotifications) {
                notification.draw(y);
                y -= OswUINotification::sDrawHeight;
            }
        }

        // Only draw overlays if enabled
        if (OswConfigAllKeys::settingDisplayOverlays.get() and (not (rootApp->getViewFlags() & OswAppV2::ViewFlags::NO_OVERLAYS) or OswConfigAllKeys::settingDisplayOverlaysForced.get()))
            drawOverlays();

        // Handle display flushing
        OswHal::getInstance()->flushCanvas();
        lastFlush = millis();
        rootApp->resetNeedsRedraw(); // indirect convention: we will clear the redraw flag after drawing (so if you set it again during onDraw(), you will need to move that to the onLoop())
    }
}

bool OswUI::getProgressActive() {
    return this->mProgressBar != nullptr;
}

void OswUI::startProgress(const char* text) {
    if (!this->getProgressActive())
        this->mProgressBar = new OswUI::OswUIProgress((short)DISP_W * 0.2f, (short)DISP_H * 0.6f, (short)DISP_W * 0.6f);
    this->mProgressText = text;
}

size_t OswUI::showNotification(std::string message, bool isPersistent) {
    std::lock_guard<std::mutex> guard(this->mNotificationsLock);  // Make sure to not modify the notifications vector during drawing
    auto notification = OswUI::OswUINotification{std::move(message), isPersistent};
    this->mNotifications.push_back(notification);
    return notification.getId();
}

void OswUI::hideNotification(size_t id) {
    std::lock_guard<std::mutex> guard(this->mNotificationsLock);  // Make sure to not modify the notifications vector during drawing
    for (auto it = this->mNotifications.begin(); it != this->mNotifications.end(); ++it) {
        if (it->getId() == id) {
            this->mNotifications.erase(it);
            return;
        }
    }
}

OswUI::OswUIProgress* OswUI::getProgressBar() {
    return this->mProgressBar;
}

void OswUI::stopProgress() {
    if (this->getProgressActive())
        return;
    this->mProgressText = "";
    delete this->mProgressBar;
    this->mProgressBar = nullptr;
}

OswUI::OswUIProgress::OswUIProgress(short x, short y, short width) : x(x), y(y), width(width) {
    this->reset();
}

OswUI::OswUIProgress::~OswUIProgress() {
}

void OswUI::OswUIProgress::setColor(const uint16_t& color) {
    this->fgColor = color;
    this->bgColor = OswUI::getInstance()->getBackgroundDimmedColor();  // TODO generate this based on passed color automatically
}

/**
 * Set the progress value of the bar.
 *
 * @param value -1 = undefined, 0 = 0%, 1 = 100%
 */
void OswUI::OswUIProgress::setProgress(const float& value) {
    if (value == -1.0f) {
        this->startValue = -1;
        this->startTime = millis();
        this->endValue = 0;
        this->endTime = millis();
    } else {
        this->startValue = (this->startValue == -1.0f ? 0.0f : this->calcValue());
        this->startTime = millis();
        this->endValue = value;
        this->endTime = millis() + 1000;  /// 1 second animation time
    }
}

void OswUI::OswUIProgress::reset() {
    this->setColor(OswUI::getInstance()->getInfoColor());
    this->setProgress(-1.0f);
}

float OswUI::OswUIProgress::calcValue() {
    const time_t now = millis();
    if (this->startValue == -1.0f)
        return (float)(millis() / 4) / 1000 - round((millis() / 4) / 1000);
    else if (now >= this->endTime)
        return this->endValue;
    else if (now <= this->startTime)
        return this->startValue;
    else {
        const time_t timeDist = this->endTime - this->startTime;
        const time_t nowDist = now - this->startTime;
        return max(0.0f, min(1.0f, this->startValue + (float)((float)nowDist / timeDist) * (this->endValue - this->startValue)));
    }
}

void OswUI::OswUIProgress::draw() {
    const float barWidth = 0.4f;  // For unknown progress bouncing
    const short barHeight = 6;
    const short barRadius = 3;
    const float value = this->calcValue();
    short bgStart = this->x;
    short bgBarWidth = this->width;
    short fgStart = this->x;
    short fgBarWidth = this->width * value;
    if (this->startValue == -1.0f) {
        const float remappedValue = -1.0f * barWidth + value * (1 + barWidth * 2);  // To allow the bar to fully hide and slide in in the same time
        const short fgEnd = min((short)(this->x + this->width), (short)(this->x + this->width * (remappedValue + barWidth / 2)));
        fgStart = min((short)(this->x + this->width), max(this->x, (short)(this->x + this->width * (remappedValue - barWidth / 2))));
        fgBarWidth = max(fgEnd - fgStart, 0);
    } else {
        const int overlap = min(value, 0.1f) * this->width;  // To prevent the radii of the two bars to be visible on their touching point
        bgStart = this->x + this->width * value - overlap;
        bgBarWidth = this->width * (1 - value) + overlap;
    }
    OswHal::getInstance()->gfx()->fillRFrame(bgStart, this->y, bgBarWidth, barHeight, barRadius, this->bgColor);
    if (fgBarWidth > barRadius * 2)
        OswHal::getInstance()->gfx()->fillRFrame(fgStart, this->y, fgBarWidth, barHeight, barRadius, this->fgColor);
    else
        // Use rectangle drawing to prevent the library to glitch out when the bar is too thin
        OswHal::getInstance()->gfx()->fillFrame(fgStart, this->y, fgBarWidth, barHeight, this->fgColor);
}

size_t OswUI::OswUINotification::count{};

OswUI::OswUINotification::OswUINotification(std::string message, bool isPersistent)
    : message{message.c_str()}, endTime{isPersistent ? millis() + 300'000 : millis() + 5'000} {
    id = count;
    ++count;
}

void OswUI::OswUINotification::draw(unsigned y) const {
    // TODO
    // * handle too long texts by adding a scroll animation?
    // * newlines may change a notifications height, so the y position should be calculated based on the current height of the notification -> return that instead of void
    Graphics2DPrint* gfx = OswHal::getInstance()->gfx();
    gfx->fillFrame(0, y, DISP_W, this->sDrawHeight, OswUI::getInstance()->getBackgroundColor());
    gfx->drawHLine(0, y, DISP_W, OswUI::getInstance()->getInfoColor());
    gfx->resetText();
    gfx->setTextCenterAligned();
    gfx->setTextCursor(DISP_W * 0.5f, y + (this->sDrawHeight * 0.5f) + 8 / 2);  // To center the text, it is assumed that one char has a height of 8 pixels
    gfx->print(this->message);
}