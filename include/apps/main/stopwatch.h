#ifndef OSW_APP_STOPWATCH_H
#define OSW_APP_STOPWATCH_H

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"
#include "sprites/text_sprite.h"

class OswAppStopWatch : public OswApp {
 public:
  OswAppStopWatch(void) { ui = OswUI::getInstance(); };
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppStopWatch(){};

 private:
    OswUI* ui;
    TextSprite *topText;
    TextSprite *bottomText;
    TextSprite *timeText;
    TextSprite *daysText;

};

#endif
