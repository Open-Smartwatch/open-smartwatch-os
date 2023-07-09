#pragma once
#include <osw_hal.h>
#include <osw_ui.h>

#include <OswAppV1.h>

#define maxLaps 36
#define lapsPerPage 5

class OswAppStopWatch : public OswApp {
  public:
    OswAppStopWatch(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    void addLap(long totalTime);
    void drawLaps();
    void drawTime(long totalTime, long y, char size);
    void drawStartButton();
    void drawPauseButton();
    void drawLapButton();
    void drawResetButton(long btn);
    void drawPageIndicator();

    ~OswAppStopWatch() {};

  private:
    OswUI* ui;
    unsigned char lapNum = 0;
    long lastLapTime = 0;
    long laps[maxLaps] = {0};
    long overall[maxLaps] = {0};
    char lapPages = 0;
    char lapPage = 0;
};
