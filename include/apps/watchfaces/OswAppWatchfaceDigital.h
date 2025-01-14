#pragma once

#include <osw_hal.h>
#include <osw_ui.h>

#include <OswAppV2.h>

class OswAppWatchfaceDigital: public OswAppV2 {
  public:
    constexpr static const char* APP_ID = "osw.wf.digital";

    const char* getAppId() override;
    const char* getAppName() override;

    void onStart() override;
    void onLoop() override;
    void onDraw() override;
    void onButton(Button id, bool up, ButtonStateNames state) override;

    inline static uint8_t getDateFormat();
    static void refreshDateFormatCache();
    static void drawSteps();
    static void digitalWatch(short timeZone, uint8_t fontSize, uint8_t dateCoordY, uint8_t timeCoordY);
    static void timeOutput(const OswTime& oswTime, bool showSecond = true);
    static void dateOutput(const OswDate& oswDate);
    static void displayWeekDay3(const char* weekday);

  private:
    static uint8_t dateFormatCache;
    time_t lastTime = 0;
};
