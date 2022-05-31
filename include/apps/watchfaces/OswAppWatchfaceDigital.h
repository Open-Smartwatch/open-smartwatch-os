#ifndef OSW_APP_WATCHFACE_DIGITAL_H
#define OSW_APP_WATCHFACE_DIGITAL_H

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"

class OswAppWatchfaceDigital : public OswApp {
  public:
    OswAppWatchfaceDigital(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppWatchfaceDigital() {};
    inline static uint8_t getDateFormat();  // Return 0 : mm/dd/yyyy 1 : dd.mm.yyyy 2 : yy.mm/dd
    static void drawSteps();
    static void digitalWatch(short timeZone, uint8_t fontSize, uint8_t dateCoordY, uint8_t timeCoordY);
    static void timeOutput(uint32_t hour, uint32_t minute, uint32_t second, bool showSecond = true);
    static void dateOutput(uint32_t yearInt, uint32_t monthInt, uint32_t dayInt);

  private:
    OswUI* ui;
};

#endif
