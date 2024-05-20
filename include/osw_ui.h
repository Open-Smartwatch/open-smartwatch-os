#ifndef OSW_UI_H
#define OSW_UI_H

#include <memory>
#include <mutex>

#include <osw_hal.h>

class OswAppV2;
class OswUI {
  public:
    class OswUIProgress {
      public:
        OswUIProgress(short x, short y, short width);
        virtual ~OswUIProgress();

        void setColor(const uint16_t& color);
        void setProgress(const float& value);
        void reset();
        void draw();
        float calcValue();

      private:
        const short x;
        const short y;
        const short width;
        uint16_t fgColor;
        uint16_t bgColor;
        float startValue = 0;
        time_t startTime = 0;
        float endValue = 0;
        time_t endTime = 0;
    };

    class OswUINotification {
      public:
        OswUINotification(std::string message, bool isPersistent);

        void draw(unsigned y) const;

        size_t getId() const {
            return id;
        }

        String getMessage() const {
            return message;
        }

        unsigned char getMessageLines() const {
            return lines;
        }

        unsigned long getEndTime() const {
            return endTime;
        }

        unsigned char getDrawHeight() const;

      private:
        unsigned char countLines(const std::string& message) const;

        static size_t count;
        const size_t id{};
        const String message{};
        const unsigned char lines;
        const unsigned long endTime{};
    };

    bool mEnableTargetFPS = true;

    OswUI();
    static OswUI* getInstance();
    static void resetInstance();

    void loop();
    void setRootApplication(OswAppV2* rootApplication);
    OswAppV2* getRootApplication();

    uint16_t getBackgroundColor();
    uint16_t getBackgroundDimmedColor();
    uint16_t getForegroundColor();
    uint16_t getForegroundDimmedColor();
    uint16_t getPrimaryColor();
    uint16_t getInfoColor();
    uint16_t getSuccessColor();
    uint16_t getWarningColor();
    uint16_t getDangerColor();

    void startProgress(const char* text);
    bool getProgressActive();
    OswUIProgress* getProgressBar();
    void stopProgress();

    size_t showNotification(std::string message, bool isPersistent);
    void hideNotification(size_t id);

    void resetTextFont();
    void resetTextColors();
    void resetTextAlignment();
    void setTextCursor(Button btn);

    unsigned int getLastFlush() const {
        return this->lastFlush;
    };
    unsigned int getLastBackgroundFlush() const {
        return this->lastBGFlush;
    };

    std::unique_ptr<std::mutex> drawLock;

  private:
    static std::unique_ptr<OswUI> instance;
    unsigned long mTargetFPS = 30;
    String mProgressText;
    OswUIProgress* mProgressBar = nullptr;
    unsigned int lastFlush = 0;
    unsigned int lastBGFlush = 0;
    std::mutex mNotificationsLock;
    std::list<OswUINotification> mNotifications;
    bool mSelfNeedsRedraw = false;
    OswAppV2* mRootApplication = nullptr;
};

#endif
