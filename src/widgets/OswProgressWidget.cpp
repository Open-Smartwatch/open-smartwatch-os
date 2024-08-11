#include "widgets/OswProgressWidget.h"
#include "OswLogger.h"
#include "osw_ui.h"

OswProgressWidget::OswProgressWidget() {
    this->setTargetProgress(-1.0); // undefined progress by default
}

bool OswProgressWidget::getNeedsRedraw() {
    std::lock_guard<std::mutex> l(this->lock);
    return this->startValue == -1.0 or // if unknown, just redraw
           this->getSmallMillis() <= endTime or // or transition is not done yet
           (this->feedback != nullptr and (this->feedback->getProgress() != this->lastPulledProgress or
                                           (this->feedback->getText() != nullptr and this->feedback->getText() != this->lastPulledText))); // or feedback changed its value
}

void OswProgressWidget::updateProgressFromFeedback() {
    if(this->feedback == nullptr)
        return; // no update if we have no feedback
    const float feedValue = this->feedback->getProgress();
    if(feedValue != this->lastPulledProgress) {
        this->setTargetProgress(feedValue);
        this->lastPulledProgress = feedValue;
    }
    const char* feedText = this->feedback->getText();
    if(feedText != nullptr) {
        if(feedText != this->lastPulledText)
            this->lastPulledText = feedText; // this will COPY the text into this widget
    } else {
        if(this->lastPulledText.size() > 0)
            this->lastPulledText = ""; // empty will be treated as not there
    }
}

void OswProgressWidget::setTargetProgress(const float& value) {
    // if unknown of specific: configure bar state accordingly
    if (value == -1.0f) {
        this->startValue = -1;
        this->startTime = this->getSmallMillis();
        this->endValue = 0;
        this->endTime = this->getSmallMillis();
    } else {
        this->startValue = (this->startValue == -1.0f ? 0.0f : this->calcValue());
        this->startTime = this->getSmallMillis();
        this->endValue = value;
        this->endTime = this->getSmallMillis() + 1000; // 1 second animation time
    }
}

void OswProgressWidget::show(std::shared_ptr<OswProgressFeedback> feedback) {
    std::lock_guard<std::mutex> l(this->lock);
    this->feedback = feedback;
}

float OswProgressWidget::calcValue() {
    const time_t now = this->getSmallMillis();
    if (this->startValue == -1.0f) // progress unknown; report 4-second looping value
        return (float)(now / 4) / 1000 - round((now / 4) / 1000);
    else if (now >= this->endTime) // we are finished
        return this->endValue;
    else if (now <= this->startTime) // progress will start soon? report start value
        return this->startValue;
    else {
        // use linear interpolation to report value until endTime
        const time_t timeDist = this->endTime - this->startTime;
        const time_t nowDist = now - this->startTime;
        const float valueDist = this->endValue - this->startValue;
        const float timePercent = (float)((float)nowDist / timeDist);
        return min(1.0f, max(0.0f, this->startValue + timePercent * valueDist));
    }
}

void OswProgressWidget::drawCircular(Graphics2DPrint* gfx, int x, int y, float scale, bool showText) {
    std::lock_guard<std::mutex> l(this->lock);
    this->updateProgressFromFeedback();

    const float value = this->calcValue();
    float arcStart;
    float arcLength;
    if (this->startValue == -1.0f) {
        arcStart = value * 360;
        arcLength = 32;
    } else {
        arcStart = 0;
        arcLength = value * 360;
    }
    const int16_t size = (this->baseDimensions * scale);
    const int16_t steps = (size / 2) * scale;
    const int16_t arcRadius = min((short) (size / 8), (short) (barHeight / 2));
    const int16_t cx = x + size / 2 + arcRadius - 2; // 2 pixel offset due to floating point errors
    const int16_t cy = y + size / 2 + arcRadius - 2; // 2 pixel offset due to floating point errors
    const uint16_t radius = size / 2 - arcRadius;
    gfx->drawArc(cx, cy, 0.0f, 360.0f, steps, radius, arcRadius, this->bgColor, false, false);
    gfx->drawArc(cx, cy, arcStart, arcStart + arcLength, steps, radius, arcRadius, this->fgColor, false, false);

    if(showText and this->lastPulledText.size()) {
        const char printMe = this->lastPulledText[0];
        gfx->setTextCursor(x + size / 2 - gfx->getCharWidth(printMe) / 2, y + size / 2 + gfx->getCharHeight(printMe) / 2);
        gfx->setTextSize(1);
        gfx->setTextColor(this->fgColor);
        // TODO: both operator are broken, if used with a single character
        // gfx->setTextMiddleAligned();
        // gfx->setTextCenterAligned();
        gfx->setTextBottomAligned();
        gfx->setTextLeftAligned();
        gfx->print(printMe);
    }


    if(this->displayDebugLines) {
        gfx->drawVLine(x, y, baseDimensions * scale, rgb565(0, 0, 255));
        gfx->drawHLine(x, y, baseDimensions * scale, rgb565(0, 0, 255));

        gfx->drawHLine(cx - radius - arcRadius, cy, arcRadius, rgb565(255, 0, 255));
        gfx->drawHLine(cx - radius, cy, radius, rgb565(255, 255, 0));
        gfx->drawHLine(cx, cy, radius, rgb565(255, 255, 0));
        gfx->drawHLine(cx + radius, cy, arcRadius, rgb565(255, 0, 255));
        gfx->drawPixel(cx, cy, rgb565(0, 255, 0));
    }
}

void OswProgressWidget::drawLinear(Graphics2DPrint* gfx, int x, int y, int width, bool showText) {
    std::lock_guard<std::mutex> l(this->lock);
    this->updateProgressFromFeedback();

    const float barWidth = 0.4; // for unknown progress bouncing
    const short barRadius = 3;
    const float value = this->calcValue();
    short bgStart = x;
    short bgBarWidth = width;
    int fgStart = x;
    short fgBarWidth = width * value;
    if (this->startValue == -1.0f) {
        const float remappedValue = -1.0f * barWidth + value * (1 + barWidth * 2);  // to allow the bar to fully hide and slide in in the same time
        const short fgEnd = min((short)(x + width), (short)(x + width * (remappedValue + barWidth / 2)));
        fgStart = min((int)(x + width), max(x, (int)(x + width * (remappedValue - barWidth / 2))));
        fgBarWidth = max(fgEnd - fgStart, 0);
    } else {
        const int overlap = min(value, 0.1f) * width;  // to prevent the radii of the two bars to be visible on their touching point
        bgStart = x + width * value - overlap;
        bgBarWidth = width * (1 - value) + overlap;
    }
    if(bgBarWidth > 0) // in some cases this value may reach 0, which would cause the library to glitch out
        gfx->fillRFrame(bgStart, y, bgBarWidth, barHeight, barRadius, this->bgColor);
    if (fgBarWidth > barRadius * 2)
        gfx->fillRFrame(fgStart, y, fgBarWidth, barHeight, barRadius, this->fgColor);
    else
        // use rectangle drawing to prevent the library to glitch out when the bar is too thin
        gfx->fillFrame(fgStart, y, fgBarWidth, barHeight, this->fgColor);

    if(showText and this->lastPulledText.size()) {
        gfx->setTextSize(1);
        gfx->setTextCursor(x + width / 2, y + barHeight + gfx->getCharHeight(this->lastPulledText[0]));
        gfx->setTextColor(this->fgColor);
        gfx->setTextBottomAligned();
        gfx->setTextCenterAligned();
        gfx->print(this->lastPulledText.c_str());
    }

    if(this->displayDebugLines) {
        gfx->drawVLine(x, y, baseDimensions, rgb565(0, 0, 255));
        gfx->drawHLine(x, y, width, rgb565(0, 0, 255));
    }
}