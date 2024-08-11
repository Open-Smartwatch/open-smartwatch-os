#pragma once

#include <osw_hal.h>
#include <OswAppV2.h>

#include "OswProgressFeedback.h"
#include "widgets/OswProgressWidget.h"

class OswAppTestProgressWidget : public OswAppV2 {
  public:
    OswAppTestProgressWidget() = default;

    const char* getAppId() override {
        return "osw.test.prog";
    };
    const char* getAppName() override {
        return "Test Progress Widget";
    };

    void onStart() override {
        OswAppV2::onStart();
        this->viewFlags = (OswAppV2::ViewFlags) (this->viewFlags | OswAppV2::ViewFlags::KEEP_DISPLAY_ON);
        this->knownButtonStates[Button::BUTTON_SELECT] = ButtonStateNames::SHORT_PRESS;
        this->knownButtonStates[Button::BUTTON_DOWN] = ButtonStateNames::SHORT_PRESS;

        feedback = std::make_shared<OswProgressFeedback>();
        widget.show(feedback); // link the widget to the feedback object
        this->step = 0;
    };

    void onLoop() override {
        OswAppV2::onLoop();
        this->needsRedraw = this->needsRedraw or widget.getNeedsRedraw();
    };

    void onDraw() override {
        OswAppV2::onDraw();

        OswUI::getInstance()->setTextCursor(Button::BUTTON_SELECT);
        OswHal::getInstance()->gfx()->print(this->step);
        OswHal::getInstance()->gfx()->print(" (");
        OswHal::getInstance()->gfx()->print(this->feedback->getProgress());
        OswHal::getInstance()->gfx()->print(")");

        OswUI::getInstance()->setTextCursor(Button::BUTTON_DOWN);
        OswHal::getInstance()->gfx()->setTextSize(0.5);
        const auto text = this->feedback->getText();
        if(text != nullptr) {
            OswHal::getInstance()->gfx()->print(text);
        } else {
            OswHal::getInstance()->gfx()->print("-");
        }

        const auto margin = 40;
        const auto padding = 10;
        auto x = margin;
        auto y = margin;
        widget.drawLinear(OswHal::getInstance()->gfx(), x, margin, DISP_W / 2);
        y += widget.baseDimensions + padding;
        widget.drawLinear(OswHal::getInstance()->gfx(), x, y, DISP_W - x * 2);
        y += widget.baseDimensions + padding;
        widget.drawCircular(OswHal::getInstance()->gfx(), x, y, 1.0f, true);
        x += widget.baseDimensions + padding;
        widget.drawCircular(OswHal::getInstance()->gfx(), x, y, 2.0f, true);
        x += widget.baseDimensions * 2.0f + padding;
        widget.drawCircular(OswHal::getInstance()->gfx(), x, y, 3.0f, true);
    };

    void onStop() override {
        OswAppV2::onStop();
    };

    void onButton(Button id, bool up, ButtonStateNames state) override {
        OswAppV2::onButton(id, up, state);
        if(!up) return;
        if(id == Button::BUTTON_SELECT) {
            this->next();
        } else if(id == Button::BUTTON_DOWN) {
            if(this->feedback->getText() == nullptr) {
                this->feedback->setText("Text");
            } else {
                this->feedback->setText(nullptr);
            }
        } else {
            throw std::logic_error("Unhandled button");
        }
    };

    bool next() {
        bool looped = false;

        ++this->step;
        if(this->step > 5) {
            this->step = 1;
            looped = true;
        }

        if(this->step == 1) {
            this->feedback->setProgress(-1.0f); // indeterminate
        } else if(this->step == 2) {
            this->feedback->setProgress(0.1); // a little bit
        } else if(this->step == 3) {
            this->feedback->setProgress(0.8); // a bit more
        } else if(this->step == 4) {
            this->feedback->setProgress(0.800001); // a little bit
        } else if(this->step == 5) {
            this->feedback->setProgress(-1.0f); // indeterminate
            this->feedback->setText("Test Text");
        }

        return looped;
    }
  private:
    std::shared_ptr<OswProgressFeedback> feedback;
    OswProgressWidget widget;
    unsigned step = 1;
};
