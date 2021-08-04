#ifndef OSW_APP_WATCHFACE_H
#define OSW_APP_WATCHFACE_H

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"
#ifdef MATRIX
#include <anim_matrix.h>
#endif

class OswAppWatchface : public OswApp {
 public:
  OswAppWatchface(void) { ui = OswUI::getInstance(); };
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppWatchface(){};

 private:
  OswUI* ui;
  void drawWatch(OswHal* hal);
#ifdef MATRIX
  AnimMatrix* matrix;
#endif
};

#endif
