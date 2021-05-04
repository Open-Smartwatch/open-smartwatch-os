#ifndef OSW_APP_TIME_FROM_WEB_H
#define OSW_APP_TIME_FROM_WEB_H

#include <osw_hal.h>

#include "osw_app.h"

class OswAppTimeFromWeb : public OswApp {
 public:
  virtual char* getName(){
      return "TimeFromWeb";
  };
  OswAppTimeFromWeb(void){};
  void setup(OswHal * hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppTimeFromWeb(){};

 private:
};

#endif