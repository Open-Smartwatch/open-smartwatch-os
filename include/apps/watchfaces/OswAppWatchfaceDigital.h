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

    inline static uint8_t getDateFormat();  // Return 0 : mm/dd/yyyy 1 : dd.mm.yyyy 2 : yy.mm/dd
    static void refreshDateFormatCache();
    static void drawSteps();
    static void digitalWatch(short timeZone, uint8_t fontSize, uint8_t dateCoordY, uint8_t timeCoordY);
    static void timeOutput(uint32_t hour, uint32_t minute, uint32_t second, bool showSecond = true);
    static void dateOutput(uint32_t yearInt, uint32_t monthInt, uint32_t dayInt);
    static void displayWeekDay3(const char* weekday);

  private:
    static uint8_t dateFormatCache;
    time_t lastTime = 0;
};
