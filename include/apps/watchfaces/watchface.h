#ifndef OSW_APP_WATCHFACE_H
#define OSW_APP_WATCHFACE_H

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"
#ifdef MATRIX
#include <anim_matrix.h>
#endif

/**
 * @brief This application manage the display of the main watchfaces.
 * 
 * The functionalities of this watchface are : 
 *      - Display the hour like a standard watch.
 *      - The step count is display with a blue arc. A full rotation is 10800 steps 
 *      - When you press the button 2 (top right) the brightness is increase
 *      - When you press the button 3 (bottom right) the brightness is decrease 
 * 
 */
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
