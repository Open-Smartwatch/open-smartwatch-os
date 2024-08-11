#pragma once

#include <atomic>
#include <OswLogger.h>

class OswProgressFeedback {
  public:
    OswProgressFeedback() {
        this->reset();
    };
    virtual ~OswProgressFeedback() = default;

    /**
     * @brief Get the numeric progress
     *
     * @return float 0-1 -> 0%-100%; -1 -> unknown
     */
    float getProgress() {
        return this->progress;
    }

    /**
     * @brief Get the text shown for as description
     *
     * @return const char*, can be nullptr if not set
     */
    const char* getText() {
        return this->text;
    }

    /**
     * @brief Check if the progress value is done (shorthand for progress == 1.0)
     *
     * @return true
     * @return false
     */
    bool done() {
        return this->progress == 1.0;
    }

    // ...and the setters for the calling party (see attribute return documentation for set-able values)
    void setProgress(float progress) {
        this->progress = progress;
    }
    void setText(const char* text) {
        this->text = text;
    }
    void reset() {
        this->text = nullptr;
        this->progress = -1.0;
    }
  private:
    std::atomic<const char*> text;
    std::atomic<float> progress;
};