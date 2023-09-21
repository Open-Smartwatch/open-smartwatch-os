#pragma once

#include "OswProgressFeedback.h"
#include <gfx_2d_print.h>

#include <memory>
#include <mutex>

class OswProgressWidget {
  public:
    static const unsigned int baseDimensions = 16; // every circular instance will be 16x16 with scale of 1.0f
    uint16_t fgColor = rgb565(200, 0, 0);
    uint16_t bgColor = rgb565(80, 80, 80);

    OswProgressWidget();
    virtual ~OswProgressWidget() = default;

    /**
     * @brief Should this widget be redrawn?
     *
     * @return true
     * @return false
     */
    bool getNeedsRedraw();

    /**
     * @brief Set the feedback instance to show (set to nullptr to hide)
     * This widget is thread-save, meaning you can't show() another feedback while drawing...
     *
     * @param feedback
     */
    void show(std::shared_ptr<OswProgressFeedback> feedback);

    /**
     * @brief Draw a circular representation of the progress feedback
     *
     * @param gfx
     * @param x
     * @param y
     * @param scale
     */
    void drawCircular(Graphics2DPrint* gfx, int x, int y, float scale = 1, bool showText = false);

    /**
     * @brief Draw a linear representation of the progress feedback
     *
     * @param gfx
     * @param xMin
     * @param xMax
     * @param y
     */
    void drawLinear(Graphics2DPrint* gfx, int x, int y, int width, bool showText = true);
  private:
    float calcValue();
    inline time_t getSmallMillis() {
        // on some systems the current time values are too big for the float-datatype
        // -> we are using a smaller value here
        return millis() % 100000;
    }
    void setTargetProgress(const float& value); // animate to a new value
    void updateProgressFromFeedback(); // check if the feedback has something new for us

    std::mutex lock;
    std::shared_ptr<OswProgressFeedback> feedback = nullptr;
    float lastPulledProgress = -1.0; // used to check if observed feedback changed
    std::string lastPulledText = ""; // used to check if observed feedback changed (using a copy, as const char* tend to disappear)
    float startValue = 0; // where the bar is currently ending
    time_t startTime = 0; // when was this state set?
    float endValue = 0; // where the bar will end...
    time_t endTime = 0; // ...and when
};