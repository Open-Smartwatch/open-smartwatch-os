#ifndef OSW_UI_H
#define OSW_UI_H

#include <osw_hal.h>

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

  void resetTextColors(void);
  void setTextCursor(Button btn);

 private:
  static OswUI instance;
  unsigned long mTargetFPS = 30;
  String mProgressText;
  OswUIProgress* mProgressBar = nullptr;
};

#endif
