#ifndef OSW_APP_STOPWATCH_H
#define OSW_APP_STOPWATCH_H

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"

#define maxLaps 36
#define lapsPerPage 5

class OswAppStopWatch : public OswApp {
 public:
  OswAppStopWatch(void) { ui = OswUI::getInstance(); };
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  void addLap(OswHal* hal, long totalTime);
  void drawLaps(OswHal* hal);
  void drawTime(OswHal* hal, long totalTime, long y, char size);
  void drawStartButton(OswHal* hal);
  void drawPauseButton(OswHal* hal);
  void drawLapButton(OswHal* hal);
  void drawResetButton(OswHal* hal, long btn);
  void drawPageIndicator(OswHal* hal);

  ~OswAppStopWatch(){};

 private:
  OswUI* ui;
  char lapNum = 0;
  long lastLapTime = 0;
  long laps[maxLaps] = {0};
  char lapPages = 0;
  char lapPage = 0;
};

#endif
