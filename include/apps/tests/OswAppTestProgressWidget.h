#pragma once

#include <osw_hal.h>
#include <OswAppV2.h>

#include "OswProgressFeedback.h"
#include "widgets/OswProgressWidget.h"

class OswAppTestProgressWidget : public OswAppV2 {
  public:
    OswAppTestProgressWidget() = default;

    const char* getAppId() override {
        return "osw.test.widget.progress";
    };
    const char* getAppName() override {
        return "Test Progress Widget";
    };

    void onStart() override {
        OswAppV2::onStart();
        this->viewFlags = (OswAppV2::ViewFlags) (this->viewFlags | OswAppV2::ViewFlags::KEEP_DISPLAY_ON);

        feedback = std::make_shared<OswProgressFeedback>();
        widget.show(feedback); // link the widget to the feedback object
        this->step = 0;
    };
    void onLoop() override {
        OswAppV2::onLoop();
        if(time(nullptr) - last > 2) {
            last = time(nullptr);
            ++this->step;
            if(this->step == 1) {
                this->feedback->setProgress(-1.0f); // indeterminate
            } else if(this->step == 2) {
                this->feedback->setProgress(0.1); // a little bit
            } else if(this->step == 3) {
                this->feedback->setProgress(0.8); // a bit more
            } else if(this->step == 4) {
                this->feedback->setProgress(0.800001); // a little bit
            } else if(this->step == 5) {
                this->feedback->setProgress(-1.0f); // back to indeterminate
            } else {
                // done
            }
            OSW_LOG_D(this->step);
        }
        this->needsRedraw = this->needsRedraw or widget.getNeedsRedraw();
    };
    void onDraw() override {
        OswAppV2::onDraw();

        widget.drawLinear(OswHal::getInstance()->gfx(), 20, 60, 80);
        widget.drawLinear(OswHal::getInstance()->gfx(), 20, 80, 160);
        widget.drawCircular(OswHal::getInstance()->gfx(), 20, 140, 1, true);
        widget.drawCircular(OswHal::getInstance()->gfx(), 80, 140, 2, true);
    };
    void onStop() override {
        OswAppV2::onStop();
    };

    bool isDone() {
        return this->step > 5;
    }
  private:
    std::shared_ptr<OswProgressFeedback> feedback;
    OswProgressWidget widget;
    time_t last = 0;
    unsigned step = 0;
};
