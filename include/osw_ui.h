#ifndef OSW_UI_H
#define OSW_UI_H

#include <osw_hal.h>

#include <memory>
#include <mutex>

class OswAppSwitcher;
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

        std::string getMessage() const {
            return message;
        }

        unsigned long getEndTime() const {
            return endTime;
        }

        const static unsigned char sDrawHeight = 16;  // EVERY notification must not be taller than this!

       private:
        size_t id{};
        static size_t count;
        const std::string message{};
        const unsigned long endTime{};
    };

    bool mEnableTargetFPS = true;

    OswUI();
    void loop(OswAppSwitcher& mainAppSwitcher, uint16_t& mainAppIndex);
    static OswUI* getInstance();

    uint16_t getBackgroundColor(void);
    uint16_t getBackgroundDimmedColor(void);
    uint16_t getForegroundColor(void);
    uint16_t getForegroundDimmedColor(void);
    uint16_t getPrimaryColor(void);
    uint16_t getInfoColor(void);
    uint16_t getSuccessColor(void);
    uint16_t getWarningColor(void);
    uint16_t getDangerColor(void);

    void startProgress(const char* text);
    bool getProgressActive();
    OswUIProgress* getProgressBar();
    void stopProgress();

    size_t showNotification(std::string message, bool isPersistent);
    void hideNotification(size_t id);

    void resetTextColors(void);
    void setTextCursor(Button btn);

    unsigned int getLastFlush() const {
        return this->lastFlush;
    };
    unsigned int getLastBackgroundFlush() const {
        return this->lastBGFlush;
    };

    std::unique_ptr<std::mutex> drawLock;

   private:
    static OswUI instance;
    unsigned long mTargetFPS = 30;
    String mProgressText;
    OswUIProgress* mProgressBar = nullptr;
    unsigned int lastFlush = 0;
    unsigned int lastBGFlush = 0;
    std::mutex mNotificationsLock;
    std::list<OswUINotification> mNotifications;
};

#endif
