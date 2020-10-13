#ifndef OSW_APP_H
#define OSW_APP_H

#include "gfx_2d.h"

class OswApp {
 public:
  OswApp(void) {}
  virtual void loop(Graphics2D* screenBuffer, bool btn1Down, bool btn2Down, bool btn3Down){};
  ~OswApp() {}
};

#endif